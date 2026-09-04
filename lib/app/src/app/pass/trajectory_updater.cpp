#include "app/pass/trajectory_updater.hpp"
#include "app/events.hpp"
#include "app/helper/trajectory.hpp"
#include "app/services.hpp"
#include "app/trajectory/loader.hpp"
#include "app/trajectory/player.hpp"
#include <cmath>
#include <core/struct/trajectory.hpp>
#include <optional>
#include <utility>

namespace VTX::App::Pass
{
	TrajectoryUpdater::TrajectoryUpdater()
	{
		REG().on_destroy<Trajectory::Loader>().connect<&TrajectoryUpdater::_onDestroyLoader>( this );
	}

	TrajectoryUpdater::~TrajectoryUpdater()
	{
		REG().on_destroy<Trajectory::Loader>().disconnect<&TrajectoryUpdater::_onDestroyLoader>( this );
		for ( const Entity entity : REG().view<Trajectory::Loader>() )
		{
			_stopLoader( entity );
		}
	}

	void TrajectoryUpdater::_onDestroyLoader( Registry &, const Entity p_entity ) { _stopLoader( p_entity ); }

	void TrajectoryUpdater::_stopLoader( const Entity p_entity )
	{
		if ( const auto * const loader = REG().try_get<Trajectory::Loader>( p_entity ) )
		{
			loader->thread->stop();
		}
	}

	namespace
	{
		/**
		 * @brief Check if a loaded frame is in the frame window.
		 */
		bool isLoadedFrameBatchCurrent(
			const Trajectory::LoadedFrameBatch &	 p_batch,
			const Trajectory::TRAJECTORY_BUFFER_MODE p_mode,
			const uint								 p_requestedFrame
		) noexcept
		{
			return p_mode == Trajectory::TRAJECTORY_BUFFER_MODE::FULL
				   || p_batch.availableFrames.contains( p_requestedFrame );
		}

		/**
		 * @brief Add loaded frame to storage.
		 */
		void applyLoadedFrames(
			const Entity			   p_entity,
			Core::Struct::Trajectory & p_trajectory,
			Trajectory::Loader &	   p_loader,
			const Trajectory::Player & p_player
		)
		{
			std::optional<Trajectory::LoadedFrameBatch> batch = p_loader.thread->takeLoadedFrames();
			if ( not batch )
			{
				return;
			}
			if ( not isLoadedFrameBatchCurrent( *batch, p_loader.mode, p_player.requestedFrameIndex ) )
			{
				return;
			}

			for ( Trajectory::LoadedFrame & loadedFrame : batch->frames )
			{
				if ( p_trajectory.frames.empty() )
				{
					break;
				}

				const size_t storageFrameIndex
					= p_loader.mode == Trajectory::TRAJECTORY_BUFFER_MODE::CIRCULAR
						  ? static_cast<size_t>( loadedFrame.index ) % p_trajectory.frames.size()
						  : static_cast<size_t>( loadedFrame.index );
				if ( storageFrameIndex < p_trajectory.frames.size() )
				{
					p_trajectory.frames[ storageFrameIndex ] = std::move( loadedFrame.positions );
				}
			}

			REG().patch<Trajectory::Loader>(
				p_entity,
				[ availableFrames = batch->availableFrames ]( Trajectory::Loader & p_component )
				{ p_component.availableFrames = availableFrames; }
			);
		}

		/**
		 * @brief Set current frame.
		 */
		bool tryUpdateFrame( const Entity p_entity, const Trajectory::Player & p_player )
		{
			return Helper::Trajectory::visitFrame(
				p_entity,
				p_player.requestedFrameIndex,
				[ p_entity ]( const Core::Struct::FrameView p_frame )
				{ HUB().trigger<Events::TrajectoryCurrentFrameChange>( { p_entity, p_frame } ); }
			);
		}

		/**
		 * @brief Get next frame index.
		 */
		uint autoplayNextFrameCount( const Trajectory::Player & p_player ) noexcept
		{
			if ( not std::isfinite( p_player.playingSpeed ) || p_player.playingSpeed <= 0.f )
			{
				return 0;
			}
			return static_cast<uint>( p_player.frameElapsedTime / p_player.playingSpeed );
		}

		/**
		 * @brief Get buffer reading direction.
		 */
		Trajectory::TRAJECTORY_READ_DIRECTION getReadDirection(
			const Trajectory::Player & p_player,
			const uint				   p_targetFrame
		) noexcept
		{
			if ( p_targetFrame != p_player.currentFrameIndex )
			{
				return p_targetFrame < p_player.currentFrameIndex ? Trajectory::TRAJECTORY_READ_DIRECTION::BACKWARD
																  : Trajectory::TRAJECTORY_READ_DIRECTION::FORWARD;
			}

			uint nextFrame = p_targetFrame;
			p_player.player.next( nextFrame );
			return nextFrame < p_targetFrame ? Trajectory::TRAJECTORY_READ_DIRECTION::BACKWARD
											 : Trajectory::TRAJECTORY_READ_DIRECTION::FORWARD;
		}

		/**
		 * @brief Ask a frame window to the loader.
		 */
		void requestFrameWindow(
			Trajectory::Loader &			 loader,
			const Core::Struct::Trajectory & p_trajectory,
			const Trajectory::Player &		 p_player,
			const uint						 p_targetFrame
		)
		{
			const Trajectory::TRAJECTORY_READ_DIRECTION direction = getReadDirection( p_player, p_targetFrame );
			loader.thread->requestFrameWindow(
				Helper::Trajectory::getFrameWindow(
					p_targetFrame, static_cast<uint>( p_trajectory.frameCount ), p_trajectory.frames.size(), direction
				),
				direction,
				loader.availableFrames
			);
		}
	} // namespace

	void TrajectoryUpdater::update( const float p_delta, const float )
	{
		// There are multiple ways to update trajectory playback, each with different tradeoffs.
		//
		// The current predictive implementation accumulates only unpaused application time:
		//
		//     frameElapsedTime += deltaTime
		//     frameCount = frameElapsedTime / playingSpeed
		//
		// When at least one frame interval has elapsed, the player advances by frameCount. This keeps playback
		// synchronized when an application update takes longer than the requested playing speed. After the frame is
		// applied, the accumulated time is reset.
		//
		// An exhaustive implementation would advance by at most one frame per application update. It would guarantee
		// that every frame is displayed, but playback could fall behind real time.
		//
		// Paused time is never accumulated, so resuming cannot skip frames to catch up with the pause duration.

		for ( const Entity entity : REG().view<Core::Struct::Trajectory, Trajectory::Player, Trajectory::Loader>() )
		{
			auto & player	  = REG().get<Trajectory::Player>( entity );
			auto & loader	  = REG().get<Trajectory::Loader>( entity );
			auto & trajectory = REG().get<Core::Struct::Trajectory>( entity );

			if ( loader.mode == Trajectory::TRAJECTORY_BUFFER_MODE::CIRCULAR
				 || loader.availableFrames.getCount() < trajectory.frameCount )
			{
				applyLoadedFrames( entity, trajectory, loader, player );
			}

			const Trajectory::FrameRange			 availableFrames = loader.availableFrames;
			const Trajectory::TRAJECTORY_BUFFER_MODE mode			 = loader.mode;

			// Nothing available.
			assert( not availableFrames.isEmpty() );

			uint nextStep			 = player.requestedFrameIndex;
			uint autoplayUpdateCount = 0;

			// Run autoplay.
			if ( nextStep == player.currentFrameIndex && not player.paused )
			{
				player.frameElapsedTime += p_delta;
				autoplayUpdateCount = autoplayNextFrameCount( player );
				player.player.next( autoplayUpdateCount, nextStep );
				if ( autoplayUpdateCount > 0 )
				{
					player.player.increment( autoplayUpdateCount );
					player.requestedFrameIndex = nextStep;
				}
			}

			// Patch and load next if needed.
			if ( nextStep != player.currentFrameIndex )
			{
				if ( not availableFrames.contains( nextStep ) )
				{
					if ( mode == Trajectory::TRAJECTORY_BUFFER_MODE::CIRCULAR )
					{
						requestFrameWindow( loader, trajectory, player, nextStep );
					}
					continue;
				}

				REG().patch<Trajectory::Player>(
					entity,
					[ entity ]( Trajectory::Player & p_player )
					{
						if ( tryUpdateFrame( entity, p_player ) )
						{
							p_player.currentFrameIndex = p_player.requestedFrameIndex;
							p_player.frameElapsedTime  = 0;
						}
					}
				);
				continue;
			}

			// Load other frames if circular.
			if ( mode == Trajectory::TRAJECTORY_BUFFER_MODE::CIRCULAR && not player.paused )
			{
				requestFrameWindow( loader, trajectory, player, player.currentFrameIndex );
			}
			// Stop.
			else if ( mode == Trajectory::TRAJECTORY_BUFFER_MODE::CIRCULAR )
			{
				loader.thread->cancelFrameRequest();
			}
		}
	}

} // namespace VTX::App::Pass
