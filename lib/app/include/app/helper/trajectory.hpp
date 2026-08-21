#ifndef __VTX_APP_HELPER_TRAJECTORY__
#define __VTX_APP_HELPER_TRAJECTORY__

#include "app/ecs.hpp"
#include "app/trajectory/player.hpp"
#include "app/trajectory/types.hpp"
#include <core/struct/trajectory.hpp>
#include <functional>

namespace VTX::IO::Writer
{
	class TrajectoryFrameGetter;
}

namespace VTX::App::Helper::Trajectory
{
	using FrameVisitor = std::function<void( Core::Struct::FrameView )>;

	/**
	 * @brief Compute the frame window to load around a target frame.
	 */
	App::Trajectory::FrameRange getFrameWindow(
		const uint,
		const uint,
		const size_t,
		const App::Trajectory::TRAJECTORY_READ_DIRECTION
	) noexcept;

	/**
	 * @brief Check whether a trajectory frame is currently available.
	 */
	bool isFrameAvailable( const Entity, const uint );

	/**
	 * @brief Call a visitor with an available trajectory frame.
	 */
	bool visitFrame( const Entity, const uint, const FrameVisitor & );

	/**
	 * @brief Call a visitor with the current trajectory frame.
	 */
	bool visitCurrentFrame( const Entity, const FrameVisitor & );

	Core::Struct::Frame getFrame( const Entity, const uint );

	/**
	 * @brief Check if a system has a multi-frame trajectory.
	 */
	bool hasMultiFrameTrajectory( const Entity );

	/**
	 * @brief Get the player data of a multi-frame trajectory.
	 */
	const App::Trajectory::Player * getPlayer( const Entity );

	/**
	 * @brief Get the range of currently available frames.
	 */
	App::Trajectory::FrameRange getAvailableFrames( const Entity );

	/**
	 * @brief Get the trajectory data for a system.
	 */
	void get( const Entity, VTX::IO::Writer::TrajectoryFrameGetter & );

} // namespace VTX::App::Helper::Trajectory

#endif
