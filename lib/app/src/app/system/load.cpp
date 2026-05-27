#include "app/system/load.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/camera.hpp"
#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/helper/preset.hpp"
#include "app/preset/name.hpp"
#include "app/services.hpp"
#include "app/system/color.hpp"
#include "app/system/deleted.hpp"
#include "app/system/representation.hpp"
#include "app/system/selection.hpp"
#include "app/system/trajectory_preparation.hpp"
#include "app/system/uid.hpp"
#include "app/system/visibility.hpp"
#include "app/uid/uid_manager.hpp"
#include <core/chemdb/atom.hpp>
#include <core/struct/topology.hpp>
#include <io/metadata.hpp>
#include <renderer/representation.hpp>
#include <util/event_hub.hpp>
#include <util/logger.hpp>
#include <util/math/aabb.hpp>
#include <util/math/range_list.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::System
{
	void deliver( PendingSystem && p_data ) noexcept;

	namespace
	{
		/**
		 * @brief Event triggered with the end of the deliver free function.
		 */
		struct EntityDelivered
		{
			Entity entity;
		};

	} // namespace

	struct SystemExtractor::_Data
	{
		PendingSystem data;
		std::latch	  synchronizer { 1 };

		_Data() = default;

		void jobFinished() { synchronizer.count_down(); }
	};

	template<typename JobFinishSignaler>
	struct DeliverSytem
	{
		void execute( JobFinishSignaler jobFinishedPtr )
		{
			deliver( std::move( jobFinishedPtr->data ) );
			jobFinishedPtr->jobFinished();
		}
	};

	void SystemExtractor::_clean() { _attributesPtr->synchronizer.count_down(); }

	SystemExtractor::SystemExtractor( FilePath p_path, IO::READER_OPTION p_options ) :
		_attributesPtr( std::make_shared<_Data>() )
	{
		_attributesPtr->data.sourcePath	  = std::move( p_path );
		_attributesPtr->data.readerOption = p_options;
	}

	SystemExtractor::SystemExtractor( FilePath p_path, std::string && p_buffer, IO::READER_OPTION p_options ) :
		SystemExtractor( std::move( p_path ), p_options )
	{
		_attributesPtr->data.buffer = std::move( p_buffer );
	}

	SystemExtractor::SystemExtractor( Entity p_entity, FilePath p_path, IO::READER_OPTION p_options ) :
		SystemExtractor( std::move( p_path ), p_options )
	{
		_attributesPtr->data.entity			= p_entity;
		_attributesPtr->data.onlyTrajectory = true;
	}

	void SystemExtractor::wait() noexcept { _attributesPtr->synchronizer.wait(); }

	uint SystemExtractor::operator()( Util::StopToken p_stopToken, Threading::OptionalThreadReference p_thread )
	{
		auto & pendingData = _attributesPtr->data;

		if ( p_thread )
		{
			p_thread.value().get().setProgressText(
				fmt::format( "Reading {}...", pendingData.sourcePath.filename().string() )
			);
		}

		if ( p_stopToken.stop_requested() )
		{
			_clean();
			return 0;
		}

		if ( pendingData.buffer )
		{
			pendingData.reader.emplace(
				std::move( pendingData.buffer.value() ), pendingData.sourcePath, pendingData.readerOption, p_stopToken
			);
		}
		else
		{
			pendingData.reader.emplace( pendingData.sourcePath, pendingData.readerOption, p_stopToken );
		}
		pendingData.reader->get(
			ECS::getCtx<Core::ChemDB::Category::Dictionary>(), pendingData.topology, pendingData.metadata
		);

		if ( p_stopToken.stop_requested() )
		{
			_clean();
			return 0;
		}

		if ( pendingData.reader->frameCount() > 1 )
		{
			pendingData.trajectoryData.emplace<System::TrajectoryFullBuffer>();
		}
		else
		{
			pendingData.trajectoryData.emplace<System::TrajectorySingleFrame>();
		}

		auto visitor = [ reader = &pendingData.reader.value() ]( auto && traj )
		{ System::prepare( traj, std::move( *reader ) ); };
		std::visit( visitor, pendingData.trajectoryData );

		if ( p_stopToken.stop_requested() )
		{
			_clean();
			return 0;
		}

		// ACTION().execute<Action::QueueAction<DeliverSytem<std::shared_ptr<_Data>>>>( _attributesPtr );
		ACTION().subscribe(
			Action::QueuedAction( DeliverSytem<std::shared_ptr<_Data>>(), std::move( _attributesPtr ) )
		);
		return 0;
	}

	void addTrajectory( const Entity & p_entity, PendingSystem & p_data ) noexcept
	{
		std::visit(
			[ &p_entity ]( auto && traj ) mutable
			{
				using TrajType = std::remove_cvref_t<decltype( traj )>;
				REG().emplace<TrajType>( p_entity, std::move( traj ) );
			},
			std::move( p_data.trajectoryData )
		);
		startAsyncTrajectoryWork( p_entity, p_data );

		std::span<const Vec3f> firstFrame = getCurrentAtomPositions( p_entity );
		// AABB (trigger update function for scene aabb).
		REG().patch<Util::Math::AABB>(
			p_entity,
			[ &firstFrame ]( Util::Math::AABB & p_aabb )
			{
				for ( auto & it_atomPos : firstFrame )
				{
					p_aabb.extend( it_atomPos, Core::ChemDB::Atom::VDW_RADIUS_MIN );
				}
			}
		);
	}

	void create( PendingSystem & p_data ) noexcept
	{
		auto & reg = REG();

		// Add components.
		auto   p_entity	 = reg.create();
		auto & data		 = reg.emplace<Core::Struct::Topology>( p_entity, std::move( p_data.topology ) );
		auto & metadata	 = reg.emplace<IO::Metadata>( p_entity, std::move( p_data.metadata ) );
		auto & transform = reg.emplace<Util::Math::Transform>( p_entity );
		auto & aabb		 = reg.emplace<Util::Math::AABB>( p_entity );
		auto & uid		 = reg.emplace<System::UID>( p_entity );

		auto & visibility	  = reg.emplace<System::Visibility>( p_entity );
		auto & selection	  = reg.emplace<System::Selection>( p_entity );
		auto & representation = reg.emplace<System::Representation>( p_entity );
		auto & color		  = reg.emplace<System::Color>( p_entity );
		auto & deleted		  = reg.emplace<System::Deleted>( p_entity );

		// UIDs: get from UID manager.
		auto & uidManager = App::UID();
		uid.residues	  = uidManager.getPickingPool().registerRange( data.getResidueCount() );
		uid.atoms		  = uidManager.getPickingPool().registerRange( data.getAtomCount() );

		addTrajectory( p_entity, p_data );

		// Visibillity: all visible.
		visibility.atoms = Util::Math::BitSet( data.getAtomCount(), true );

		// Selection: nothing selected.
		selection.atoms = Util::Math::BitSet( data.getAtomCount() );

		// Deleted: nothing deleted.
		deleted.atoms = {};

		// Color: set default color scheme.
		color.colorSchemeAtoms[ Renderer::E_COLOR_SCHEME::ATOM ] = Core::Struct::IndexRangeList( data.getAtomRange() );

		// Representation: set default representation.
		// TODO: configure default representation in settings?
		representation.presetAtoms
			[ Helper::Preset::getByName<Renderer::Representation>( "Sticks and Ribbons" )
				  .value_or( ECS::getFirstEntityOnlyWithComponents<Preset::Name, Renderer::Representation>() ) ]
			= Core::Struct::IndexRangeList( data.getAtomRange() );

		// Trigger system load.
		HUB().trigger<Events::SystemLoad>( { p_entity } );

		// Orient.
		ACTION().execute<Action::Camera::Orient>( aabb );
	}

	void deliver( PendingSystem && p_data ) noexcept
	{
		if ( p_data.onlyTrajectory && p_data.entity )
		{
			auto topology = REG().try_get<Core::Struct::Topology>( p_data.entity.value() );
			if ( topology && topology->getAtomCount() == p_data.topology.getAtomCount() )
			{
				addTrajectory( *p_data.entity, p_data );

				if ( auto uid = REG().try_get<System::UID>( *p_data.entity ) )
				{
					// Trigger trajectory event.
					HUB().trigger<Events::TrajectoryLoad>( { *p_data.entity,
															 getCurrentAtomPositions( *p_data.entity ) } );
				}
			}
			else
			{
				VTX::VTX_ERROR(
					"File {} has different atom count. ({}/{})",
					p_data.sourcePath.string(),
					topology->getAtomCount(),
					p_data.topology.getAtomCount()
				);
			}
		}
		else
		{
			create( p_data );
		}
	}

} // namespace VTX::App::System
