#include "app/extractor/system.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/camera.hpp"
#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/generic/name.hpp"
#include "app/helper/preset.hpp"
#include "app/helper/trajectory.hpp"
#include "app/services.hpp"
#include "app/system/color.hpp"
#include "app/system/representation.hpp"
#include "app/system/selection.hpp"
#include "app/system/trajectory_loader.hpp"
#include "app/system/trajectory_player.hpp"
#include "app/system/uid.hpp"
#include "app/system/visibility.hpp"
#include "app/trajectory/types.hpp"
#include "app/uid/uid_manager.hpp"
#include <core/struct/topology.hpp>
#include <core/struct/trajectory.hpp>
#include <io/metadata.hpp>
#include <io/reader.hpp>
#include <latch>
#include <limits>
#include <memory>
#include <optional>
#include <renderer/representation.hpp>
#include <stdexcept>
#include <util/event_hub.hpp>
#include <util/logger.hpp>
#include <util/math/aabb.hpp>
#include <util/math/grid.hpp>
#include <util/math/range_list.hpp>
#include <util/math/transform.hpp>
#include <util/thread/thread_manager.hpp>
#include <utility>

namespace VTX::App::Extractor
{
	struct Pending
	{
		std::optional<Entity>								   entity;
		bool												   onlyTrajectory = false;
		FilePath											   sourcePath;
		IO::READER_OPTION									   readerOption;
		TrajectoryBufferSettings							   trajectoryBufferSettings;
		std::optional<std::string>							   buffer;
		std::optional<IO::SystemReader>						   reader;
		Core::Struct::Topology								   topology;
		IO::Metadata										   metadata;
		Util::Math::AABB									   aabb;
		Util::Math::Grid<Index>								   atomGrid;
		Core::Struct::Trajectory							   trajectory;
		std::optional<App::System::TrajectoryPlayer>		   trajectoryPlayer;
		std::optional<App::Trajectory::TRAJECTORY_BUFFER_MODE> trajectoryBufferMode;
	};

	void deliver( Pending && p_data );

	namespace
	{
		size_t _getCurrentAtomPositionCount( const Core::Struct::Trajectory & p_trajectory ) noexcept
		{
			if ( p_trajectory.frames.empty() )
			{
				return 0;
			}

			return p_trajectory.frames[ 0 ].size();
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
			try
			{
				deliver( std::move( jobFinishedPtr->data ) );
			}
			catch ( ... )
			{
				jobFinishedPtr->jobFinished();
				throw;
			}
			jobFinishedPtr->jobFinished();
		}
	};

	void System::_clean() { _attributesPtr->synchronizer.count_down(); }

	System::System( FilePath p_path, IO::READER_OPTION p_options, const TrajectoryBufferSettings p_bufferSettings ) :
		_attributesPtr( std::make_shared<_Data>() )
	{
		_attributesPtr->data.sourcePath				  = std::move( p_path );
		_attributesPtr->data.readerOption			  = p_options;
		_attributesPtr->data.trajectoryBufferSettings = p_bufferSettings;
	}

	System::System(
		FilePath					   p_path,
		std::string &&				   p_buffer,
		IO::READER_OPTION			   p_options,
		const TrajectoryBufferSettings p_bufferSettings
	) : System( std::move( p_path ), p_options, p_bufferSettings )
	{ _attributesPtr->data.buffer = std::move( p_buffer ); }

	System::System(
		const Entity				   p_entity,
		FilePath					   p_path,
		IO::READER_OPTION			   p_options,
		const TrajectoryBufferSettings p_bufferSettings
	) : System( std::move( p_path ), p_options, p_bufferSettings )
	{
		_attributesPtr->data.entity			= p_entity;
		_attributesPtr->data.onlyTrajectory = true;
	}

	void System::wait() noexcept { _attributesPtr->synchronizer.wait(); }

	uint System::operator()( Util::Thread::StopToken p_stopToken, Util::Thread::OptionalThreadReference p_thread )
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

			pendingData.trajectory.frameCount = pendingData.reader->frameCount();
			if ( pendingData.trajectory.frameCount == 0 )
			{
				throw std::runtime_error( "Trajectory contains no frame." );
			}
			if ( pendingData.trajectory.frameCount > std::numeric_limits<uint>::max() )
			{
				throw std::length_error( "Trajectory frame count exceeds the supported range." );
			}
			Core::Struct::Frame firstFrame;
			pendingData.reader->get( firstFrame, 0 );
			if ( pendingData.trajectory.frameCount > 1 )
			{
				pendingData.trajectoryBufferMode.emplace( pendingData.trajectoryBufferSettings.selectMode(
					firstFrame.size(), pendingData.trajectory.frameCount
				) );
			}
			const size_t frameStorageCount
				= pendingData.trajectoryBufferMode
					  ? pendingData.trajectoryBufferSettings.getStorageFrameCount(
							*pendingData.trajectoryBufferMode, pendingData.trajectory.frameCount
						)
					  : pendingData.trajectory.frameCount;
			pendingData.trajectory.frames.resize( frameStorageCount );
			pendingData.trajectory.frames[ 0 ] = std::move( firstFrame );
			if ( pendingData.trajectory.frameCount > 1 )
			{
				pendingData.trajectoryPlayer.emplace();
				pendingData.trajectoryPlayer->player.setStepCount(
					static_cast<uint>( pendingData.trajectory.frameCount )
				);
			}

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

	void addTrajectory( const Entity & p_entity, Pending & p_data )
	{
		auto & registry = REG();
		registry.remove<App::System::TrajectoryLoader>( p_entity );
		registry.remove<App::System::TrajectoryPlayer>( p_entity );
		registry.emplace_or_replace<Core::Struct::Trajectory>( p_entity, std::move( p_data.trajectory ) );

		if ( p_data.trajectoryBufferMode && p_data.trajectoryPlayer )
		{
			registry.emplace<App::System::TrajectoryPlayer>( p_entity, std::move( *p_data.trajectoryPlayer ) );
			registry.emplace<App::System::TrajectoryLoader>(
				p_entity,
				THREAD().createThread<App::Trajectory::LoaderThread>(
					std::move( p_data.reader.value() ), *p_data.trajectoryBufferMode
				),
				*p_data.trajectoryBufferMode
			);
		}
	}

	void create( Pending & p_data )
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

	void deliver( Pending && p_data )
	{
		if ( p_data.onlyTrajectory && p_data.entity )
		{
			if ( not REG().valid( *p_data.entity ) || not REG().all_of<Core::Struct::Topology>( *p_data.entity ) )
			{
				return;
			}

			auto &		 topology		  = REG().get<Core::Struct::Topology>( p_data.entity.value() );
			const size_t pendingAtomCount = _getCurrentAtomPositionCount( p_data.trajectory );

			if ( topology.getAtomCount() == pendingAtomCount )
			{
				addTrajectory( *p_data.entity, p_data );

				if ( auto uid = REG().try_get<App::System::UID>( *p_data.entity ) )
				{
					App::Helper::Trajectory::visitCurrentFrame(
						*p_data.entity,
						[ entity = *p_data.entity ]( const Core::Struct::FrameView p_frame )
						{ HUB().trigger<Events::TrajectoryLoad>( { entity, p_frame } ); }
					);
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
