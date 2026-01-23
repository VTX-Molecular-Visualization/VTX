#include "app/pass/trajectory_updater.hpp"
#include "app/services.hpp"
#include "app/system/trajectory.hpp"
#include "app/system/uid.hpp"
#include <renderer/renderer.hpp>

namespace VTX::App::Pass
{
	TrajectoryUpdater::TrajectoryUpdater() {}

	namespace
	{

		System::GenericTrajectory & genericTrajectory( System::TrajectoryFullBuffer & p_traj )
		{
			return p_traj.genericData;
		}

		const System::GenericTrajectory & genericTrajectory( const System::TrajectoryFullBuffer & p_traj )
		{
			return p_traj.genericData;
		}

		bool tryUpdateFrame( const ECS::Entity & entity, System::TrajectoryFullBuffer & p_traj )
		{
			if ( p_traj.lastFrameAvailable < p_traj.genericData.requestedFrameIndex )
				return false;

			RENDERER().setSystemPosition(
				REG().get<System::UID>( entity ).system,
				p_traj.frameCollection[ p_traj.genericData.requestedFrameIndex ]
			);
			return true;
		}
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
		/**
		 * @brief Update frame for every trajectory of the input type
		 * @tparam TrajectoryT type of trajectory
		 * @param p_elapsedTime Time since program start
		 */
		template<typename TrajectoryT>
		void updateTrajectoresPosition( const float p_elapsedTime )
		{
			for ( ECS::Entity it_entity : REG().view<TrajectoryT>() )
			{
				{
					const TrajectoryT & traj = REG().get<TrajectoryT>( it_entity );
					if ( not shouldUpdateFrame( genericTrajectory( traj ), p_elapsedTime ) )
						continue;
				}

				REG().patch<TrajectoryT>(
					it_entity,
					[ &it_entity, &p_elapsedTime ]( TrajectoryT & traj )
					{
						if ( tryUpdateFrame( it_entity, traj ) )
						{
							System::GenericTrajectory & genericData = genericTrajectory( traj );
							genericData.currentFrameIndex			= genericData.requestedFrameIndex;
							genericData.lastFrameUpdateTime			= p_elapsedTime;
						}
					}
				);
			}
		}
	} // namespace

	void TrajectoryUpdater::update( const float p_elapsedTime, const float )
	{
		updateTrajectoresPosition<System::TrajectoryFullBuffer>( p_elapsedTime );
	}

} // namespace VTX::App::Pass
