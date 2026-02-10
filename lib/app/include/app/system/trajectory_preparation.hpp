#ifndef __VTX_APP_SYSTEM_TRAJECTORY_PREPARATION__
#define __VTX_APP_SYSTEM_TRAJECTORY_PREPARATION__

#include "app/system/load.hpp"
#include "app/system/trajectory.hpp"
#include <io/reader/system.hpp>

namespace VTX::App::System
{

	/**
	 * @brief Fill trajectory data structure with one frame. Leave the trajectory at a state ready for rendering
	 * @param p_entity
	 * @param p_trajectory
	 * @param p_loader
	 */
	void prepare( TrajectoryFullBuffer & p_trajectory, IO::Reader::System && p_loader ) noexcept;
	void prepare( TrajectorySingleFrame & p_trajectory, IO::Reader::System && p_loader ) noexcept;

	void startAsyncWork( const ECS::Entity &, PendingSystem & ) noexcept;
} // namespace VTX::App::System
#endif
