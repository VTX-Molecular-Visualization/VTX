#ifndef __VTX_APP_SYSTEM_TRAJECTORY_PREPARATION__
#define __VTX_APP_SYSTEM_TRAJECTORY_PREPARATION__

#include "app/system/trajectory.hpp"
#include <io/reader/system.hpp>

namespace VTX::App::System
{

	/**
	 * @brief Fill trajectory data structure up to the first frame, then start threaded frame loading.
	 * @param p_entity
	 * @param p_trajectory
	 * @param p_loader
	 */
	void prepare( ECS::Entity p_entity, TrajectoryFullBuffer & p_trajectory, IO::Reader::System && p_loader ) noexcept;
} // namespace VTX::App::System
#endif
