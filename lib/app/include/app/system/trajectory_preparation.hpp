#ifndef __VTX_APP_SYSTEM_TRAJECTORY_PREPARATION__
#define __VTX_APP_SYSTEM_TRAJECTORY_PREPARATION__

#include "app/ecs.hpp"
#include "app/system/trajectory.hpp"
#include <io/reader.hpp>

namespace VTX::App::System
{

	/**
	 * @brief Fill trajectory data structure with one frame. Leave the trajectory at a state ready for rendering
	 * @param p_entity
	 * @param p_trajectory
	 * @param p_loader
	 */
	void prepare( TrajectoryFullBuffer & p_trajectory, IO::SystemReader && p_loader );
	void prepare( TrajectorySingleFrame & p_trajectory, IO::SystemReader && p_loader );

	/**
	 * @brief Lauch asynchronous work related to the trajectory, if any.
	 * @param
	 * @param
	 */
	void startAsyncTrajectoryWork( const Entity &, IO::SystemReader && );
} // namespace VTX::App::System
#endif
