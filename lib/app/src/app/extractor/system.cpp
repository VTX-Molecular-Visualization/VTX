#include "app/extractor/system.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/camera.hpp"
#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/generic/name.hpp"
#include "app/helper/preset.hpp"
#include "app/services.hpp"
#include "app/system/color.hpp"
#include "app/system/representation.hpp"
#include "app/system/selection.hpp"
#include "app/system/trajectory_preparation.hpp"
#include "app/system/uid.hpp"
#include "app/system/visibility.hpp"
#include "app/uid/uid_manager.hpp"
#include <core/struct/topology.hpp>
#include <io/metadata.hpp>
#include <io/reader.hpp>
#include <latch>
#include <optional>
#include <renderer/representation.hpp>
#include <util/event_hub.hpp>
#include <util/logger.hpp>
#include <util/math/aabb.hpp>
#include <util/math/grid.hpp>
#include <util/math/range_list.hpp>
#include <util/math/transform.hpp>
#include <variant>

namespace VTX::App::Extractor
{
	struct Pending
	{
		std::optional<Entity>																entity;
		bool																				onlyTrajectory = false;
		FilePath																			sourcePath;
		IO::READER_OPTION																	readerOption;
		std::optional<std::string>															buffer;
		std::optional<IO::SystemReader>														reader;
		Core::Struct::Topology																topology;
		IO::Metadata																		metadata;
		Util::Math::AABB																	aabb;
		Util::Math::Grid<Index>																atomGrid;
		std::variant<App::System::TrajectorySingleFrame, App::System::TrajectoryFullBuffer> trajectoryData;
	};

	void deliver( Pending && p_data ) noexcept;

	namespace
	{
		size_t _getCurrentAtomPositionCount( const App::System::TrajectorySingleFrame & p_trajectory ) noexcept
		{ return p_trajectory.atomPositions.size(); }

		size_t _getCurrentAtomPositionCount( const App::System::TrajectoryFullBuffer & p_trajectory ) noexcept
		{
			if ( p_trajectory.frameCollection.empty() )
			{
				return 0;
			}

			return p_trajectory.frameCollection[ p_trajectory.genericData.currentFrameIndex ].size();
		}

		/**
		 * @brief Event triggered with the end of the deliver free function.
		 */
		struct EntityDelivered
		{
			Entity entity;
		};

	} // namespace

	struct System::_Data
	{
		Pending	   data;
		std::latch synchronizer { 1 };

		_Data() = default;

		void jobFinished() { synchronizer.count_down(); }
	};

	template<typename JobFinishSignaler>
	struct DeliverSystem
	{
		void execute( JobFinishSignaler jobFinishedPtr )
		{
			deliver( std::move( jobFinishedPtr->data ) );
			jobFinishedPtr->jobFinished();
		}
	};

	void System::_clean() { _attributesPtr->synchronizer.count_down(); }

	System::System( FilePath p_path, IO::READER_OPTION p_options ) : _attributesPtr( std::make_shared<_Data>() )
	{
		_attributesPtr->data.sourcePath	  = std::move( p_path );
		_attributesPtr->data.readerOption = p_options;
	}

	System::System( FilePath p_path, std::string && p_buffer, IO::READER_OPTION p_options ) :
		System( std::move( p_path ), p_options )
	{ _attributesPtr->data.buffer = std::move( p_buffer ); }

	System::System( Entity p_entity, FilePath p_path, IO::READER_OPTION p_options ) :
		System( std::move( p_path ), p_options )
	{
		_attributesPtr->data.entity			= p_entity;
		_attributesPtr->data.onlyTrajectory = true;
	}

	void System::wait() noexcept { _attributesPtr->synchronizer.wait(); }

	uint System::operator()( Util::StopToken p_stopToken, Threading::OptionalThreadReference p_thread )
	{
		try
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
					std::move( pendingData.buffer.value() ),
					pendingData.sourcePath,
					pendingData.readerOption,
					p_stopToken
				);
			}
			else
			{
				pendingData.reader.emplace( pendingData.sourcePath, pendingData.readerOption, p_stopToken );
			}
			if ( not pendingData.onlyTrajectory )
			{
				pendingData.reader->get(
					ECS::getCtx<Core::ChemDB::Category::Dictionary>(),
					pendingData.topology,
					pendingData.metadata,
					pendingData.aabb,
					pendingData.atomGrid
				);
			}

			if ( p_stopToken.stop_requested() )
			{
				_clean();
				return 0;
			}

			if ( pendingData.reader->frameCount() > 1 )
			{
				pendingData.trajectoryData.emplace<App::System::TrajectoryFullBuffer>();
			}
			else
			{
				pendingData.trajectoryData.emplace<App::System::TrajectorySingleFrame>();
			}

			auto visitor = [ reader = &pendingData.reader.value() ]( auto && traj )
			{ App::System::prepare( traj, std::move( *reader ) ); };
			std::visit( visitor, pendingData.trajectoryData );

			if ( p_stopToken.stop_requested() )
			{
				_clean();
				return 0;
			}

			auto attributesCopy = _attributesPtr; // Keep a copy so wait() can still access the latch.
			ACTION().subscribe(
				Action::QueuedAction( DeliverSystem<std::shared_ptr<_Data>>(), std::move( attributesCopy ) )
			);
			return 0;
		}
		catch ( ... )
		{
			_clean();
			throw;
		}
	}

	void addTrajectory( const Entity & p_entity, Pending & p_data ) noexcept
	{
		std::visit(
			[ &p_entity ]( auto && traj ) mutable
			{
				using TrajType = std::remove_cvref_t<decltype( traj )>;
				REG().emplace<TrajType>( p_entity, std::move( traj ) );
			},
			std::move( p_data.trajectoryData )
		);
		App::System::startAsyncTrajectoryWork( p_entity, std::move( p_data.reader.value() ) );
	}

	void create( Pending & p_data ) noexcept
	{
		auto & reg = REG();

		// Add components.
		auto   p_entity	 = reg.create();
		auto & data		 = reg.emplace<Core::Struct::Topology>( p_entity, std::move( p_data.topology ) );
		auto & metadata	 = reg.emplace<IO::Metadata>( p_entity, std::move( p_data.metadata ) );
		auto & transform = reg.emplace<Util::Math::Transform>( p_entity );
		auto & aabb		 = reg.emplace<Util::Math::AABB>( p_entity, std::move( p_data.aabb ) );
		reg.emplace<Util::Math::Grid<Index>>( p_entity, std::move( p_data.atomGrid ) );
		auto & uid = reg.emplace<App::System::UID>( p_entity );

		auto & visibility	  = reg.emplace<App::System::Visibility>( p_entity );
		auto & selection	  = reg.emplace<App::System::Selection>( p_entity );
		auto & representation = reg.emplace<App::System::Representation>( p_entity );
		auto & color		  = reg.emplace<App::System::Color>( p_entity );

		// UIDs: get from UID manager.
		auto & uidManager = App::UID();
		uid.residues	  = uidManager.getPickingPool().registerRange( data.getResidueCount() );
		uid.atoms		  = uidManager.getPickingPool().registerRange( data.getAtomCount() );

		addTrajectory( p_entity, p_data );

		// Visibillity: all visible.
		visibility.atoms = Util::Math::BitSet( data.getAtomCount(), true );

		// Selection: nothing selected.
		selection.atoms = Util::Math::BitSet( data.getAtomCount() );

		// Color: set default color scheme.
		color.colorSchemeAtoms[ Renderer::E_COLOR_SCHEME::ATOM ] = Core::Struct::IndexRangeList( data.getAtomRange() );
		color.colorSchemeSecondaryStructureResidues[ Renderer::E_COLOR_SCHEME_SECONDARY_STRUCTURE::STRUCTURE ]
			= Core::Struct::IndexRangeList( Core::Struct::IndexRange::fromFirstCount( 0, data.getResidueCount() ) );

		// Representation: set default representation.
		// TODO: configure default representation in settings?
		representation.presetAtoms
			[ Helper::Preset::getByName<Renderer::Representation>( "Sticks and Ribbons" )
				  .value_or( ECS::getFirstEntityOnlyWithComponents<Generic::Name, Renderer::Representation>() ) ]
			= Core::Struct::IndexRangeList( data.getAtomRange() );

		// Trigger system load.
		HUB().trigger<Events::SystemLoad>( { p_entity } );

		// Orient.
		ACTION().execute<Action::Camera::Orient>( aabb );
	}

	void deliver( Pending && p_data ) noexcept
	{
		if ( p_data.onlyTrajectory && p_data.entity )
		{
			auto &		 topology		  = REG().get<Core::Struct::Topology>( p_data.entity.value() );
			const size_t pendingAtomCount = std::visit(
				[]( const auto & p_trajectory ) { return _getCurrentAtomPositionCount( p_trajectory ); },
				p_data.trajectoryData
			);

			if ( topology.getAtomCount() == pendingAtomCount )
			{
				addTrajectory( *p_data.entity, p_data );

				if ( auto uid = REG().try_get<App::System::UID>( *p_data.entity ) )
				{
					// Trigger trajectory event.
					HUB().trigger<Events::TrajectoryLoad>( { *p_data.entity,
															 App::System::getCurrentAtomPositions( *p_data.entity ) } );
				}
			}
			else
			{
				VTX::VTX_ERROR(
					"File {} has different atom count. ({}/{})",
					p_data.sourcePath.string(),
					topology.getAtomCount(),
					pendingAtomCount
				);
			}
		}
		else
		{
			create( p_data );
		}
	}

} // namespace VTX::App::Extractor
