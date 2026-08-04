#include "app/pass/trajectory_updater.hpp"
#include "app/services.hpp"
#include "app/system/trajectory.hpp"
#include "app/system/uid.hpp"
#include "app/threading/thread_manager.hpp"
#include <algorithm>

namespace VTX::App::Pass
{
	TrajectoryUpdater::TrajectoryUpdater()
	{ REG().on_destroy<System::TrajectoryFullBuffer>().connect<&TrajectoryUpdater::_onDestroyTrajectory>( this ); }

	bool TrajectoryUpdater::_tryUpdateFrame( const Entity & entity, System::TrajectoryFullBuffer & p_traj ) noexcept
	{
		if ( p_traj.lastFrameAvailable < p_traj.genericData.requestedFrameIndex )
		{
			return false;
		}

		// Trigger trajectory event.
		HUB().trigger<Events::TrajectoryLoad>( { entity,
												 p_traj.frameCollection[ p_traj.genericData.requestedFrameIndex ] } );

		return true;
	}

	void TrajectoryUpdater::_onDestroyTrajectory( Registry &, Entity p_entity )
	{
		if ( auto traj = REG().try_get<System::TrajectoryFullBuffer>( p_entity ) )
		{
			// If the trajectory worker is still doing stuff, stop it and join the thread before destroying the
			// component.
			Threading::BaseThread * thr = nullptr;
			THREAD().get( traj->threadId, thr );
			if ( thr )
			{
				thr->stop();
				thr->wait();
			}
		}
	}

	namespace
	{
		/**
		 * @brief Returns whether the new positions has been set to the requestedFrameIndex
		 *
		 * @param entity
		 * @param p_traj
		 * @return
		 */
		bool tryUpdateFrame( const Entity & entity, System::TrajectoryFullBuffer & p_traj ) noexcept
		{
			if ( p_traj.lastFrameAvailable < p_traj.genericData.requestedFrameIndex )
			{
				return false;
			}

			// Trigger trajectory event.
			HUB().trigger<Events::TrajectoryLoad>(
				{ entity, p_traj.frameCollection[ p_traj.genericData.requestedFrameIndex ] }
			);

			return true;
		}

		uint autoplayNextFrameCount( const System::GenericTrajectory & p_traj ) noexcept
		{ return static_cast<uint>( p_traj.frameElapsedTime / p_traj.playingSpeed ); }

		template<typename TrajectoryT>
		System::GenericTrajectory & genericData( TrajectoryT & p_ )
		{ return p_.genericData; }

		/**
		 * @brief Update the trajectory to request a realistic frame, using availability date
		 * @param p_expectedNextStep Frame that should be used if requirements are met
		 * @param p_traj trajectory to by modified
		 */
		inline void setRequestedFrameIndex( const uint & p_expectedNextStep, System::TrajectoryFullBuffer & p_traj )
		{ p_traj.genericData.requestedFrameIndex = std::min( p_expectedNextStep, p_traj.lastFrameAvailable ); }

		/**
		 * @brief Update frame for every trajectory of the input type
		 * @tparam TrajectoryT
		 *
		 * @param p_delta Time since the previous update
		 */
		template<typename TrajectoryT>
		void updateTrajectoresPosition( const float p_delta )
		{
			for ( Entity it_entity : REG().view<TrajectoryT>() )
			{
				System::GenericTrajectory & genericTraj = genericData( REG().get<TrajectoryT>( it_entity ) );
				const auto &				player		= genericTraj.player;

				uint nextStep				  = genericTraj.requestedFrameIndex;
				uint autoplayUpdateIncrNumber = 0;
				if ( nextStep == genericTraj.currentFrameIndex
					 && not genericTraj.paused ) // If there is no outside demand on setting the
												 // current frame, we use the autoplay.
				{
					genericTraj.frameElapsedTime += p_delta;
					autoplayUpdateIncrNumber = autoplayNextFrameCount( genericTraj );
					player.next( autoplayUpdateIncrNumber, nextStep );
				}
				if ( nextStep == genericTraj.currentFrameIndex )
				{
					continue;
				}

				REG().patch<TrajectoryT>(
					it_entity,
					[ &nextStep, &it_entity, &autoplayUpdateIncrNumber ]( TrajectoryT & traj )
					{
						System::GenericTrajectory & trajGenericData = genericData( traj );
						if ( autoplayUpdateIncrNumber > 0 )
						{
							trajGenericData.player.increment( autoplayUpdateIncrNumber );
						}

						setRequestedFrameIndex( nextStep, traj );
						if ( tryUpdateFrame( it_entity, traj ) )
						{
							trajGenericData.currentFrameIndex = trajGenericData.requestedFrameIndex;
							trajGenericData.frameElapsedTime  = 0;
						}
					}
				);
			}
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

		updateTrajectoresPosition<System::TrajectoryFullBuffer>( p_delta );
	}

} // namespace VTX::App::Pass
