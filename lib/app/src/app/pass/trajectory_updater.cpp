#include "app/pass/trajectory_updater.hpp"
#include "app/services.hpp"
#include "app/system/trajectory.hpp"
#include "app/system/uid.hpp"
#include "app/threading/thread_manager.hpp"
#include <renderer/renderer.hpp>

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

		uint autoplayNextFrameCount( const System::GenericTrajectory & p_traj, const float p_elapsedTime ) noexcept
		{ return static_cast<uint>( ( p_elapsedTime - p_traj.lastFrameUpdateTime ) / p_traj.playingSpeed ); }

		/**
		 * @brief Return true if the frame should be updated.
		 * @param p_traj trajectory data
		 * @param p_elapsedTime elapsed time since software start
		 * @return
		 */
		bool shouldUpdateFrame( const System::GenericTrajectory & p_traj, const float & p_elapsedTime )
		{
			return p_traj.currentFrameIndex != p_traj.requestedFrameIndex
				   && p_traj.lastFrameUpdateTime + p_traj.playingSpeed > p_elapsedTime;
		}

		template<typename TrajectoryT>
		System::GenericTrajectory & genericData( TrajectoryT & p_ )
		{ return p_.genericData; }

		/**
		 * @brief Update frame for every trajectory of the input type
		 * @tparam TrajectoryT type of trajectory
		 * @param p_elapsedTime Time since program start
		 */
		template<typename TrajectoryT>
		void updateTrajectoresPosition( const float p_elapsedTime )
		{
			for ( Entity it_entity : REG().view<TrajectoryT>() )
			{
				System::GenericTrajectory * genericTrajPtr = nullptr;
				System::get( it_entity, genericTrajPtr );
				if ( genericTrajPtr == nullptr )
				{
					continue;
				}
				auto & player = genericTrajPtr->player;

				uint nextStep				  = genericTrajPtr->requestedFrameIndex;
				uint autoplayUpdateIncrNumber = 0;
				if ( nextStep == genericTrajPtr->currentFrameIndex
					 && not genericTrajPtr->paused ) // If there is no outside demand on setting the
													 // current frame, we use the autoplay
				{
					autoplayUpdateIncrNumber = autoplayNextFrameCount( *genericTrajPtr, p_elapsedTime );
					player.next( autoplayUpdateIncrNumber, nextStep );
				}
				if ( nextStep == genericTrajPtr->currentFrameIndex )
				{
					continue;
				}

				REG().patch<TrajectoryT>(
					it_entity,
					[ &nextStep, &it_entity, &p_elapsedTime, &autoplayUpdateIncrNumber ]( TrajectoryT & traj )
					{
						System::GenericTrajectory & trajGenericData = genericData( traj );
						trajGenericData.player.increment(
							autoplayUpdateIncrNumber
						); // std::min is for the exhaustive algorithm. It means that if it is 0, no increment is made.
						   // If >=1, only one increment is made. A predictive algorithm would be not using std::min

						trajGenericData.requestedFrameIndex = nextStep;
						if ( tryUpdateFrame( it_entity, traj ) )
						{
							trajGenericData.currentFrameIndex	= trajGenericData.requestedFrameIndex;
							trajGenericData.lastFrameUpdateTime = p_elapsedTime;
						}
					}
				);
			}
		}
	} // namespace

	void TrajectoryUpdater::update( const float p_delta, const float p_elapsedTime )
	{
		/*
		There is multiple ways to implement trajectory frame update. Each comes with pros and cons.
		The way currently implemented will be referred as "exhaustive".
		It governs how the autoplay will decide what the nextframe is :
			We check if

				elapsedTime > lastUpdateTime + playingSpeed

			If it is : the next frame
			is called. What it means is that if the delta between updates is multiple time bigger than the playing
		speed, this algorithm won't try to catch up but will display every single frame.

		Another implementation would be "predictive" where we skip N frames for

			lastUpdateTime + (N - 1) * playingSpeed < elapsedTime < lastUpdateTime + N * playingSpeed

		The predictive algorithm would enforce trajectory synchronisation if applicable, while the exhaustive algorithm
		makes sure every step is displayed.

		*/

		updateTrajectoresPosition<System::TrajectoryFullBuffer>( p_elapsedTime );
	}

} // namespace VTX::App::Pass
