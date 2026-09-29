#ifndef __VTX_APP_HELPER_TRAJECTORY__
#define __VTX_APP_HELPER_TRAJECTORY__

#include "app/ecs.hpp"
#include "app/trajectory/player.hpp"
#include "app/trajectory/types.hpp"
#include <core/struct/trajectory.hpp>
#include <functional>
#include <optional>

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
		uint,
		uint,
		size_t,
		App::Trajectory::TRAJECTORY_READ_DIRECTION
	) noexcept;

	/**
	 * @brief Compute the frame storage index.
	 */
	std::optional<size_t> resolveStorageFrameIndex(
		uint,
		size_t,
		App::Trajectory::TRAJECTORY_BUFFER_MODE,
		uint,
		uint
	) noexcept;

	/**
	 * @brief Check whether a trajectory frame is currently available.
	 */
	bool isFrameAvailable( Entity, uint );

	/**
	 * @brief Call a visitor with an available trajectory frame.
	 */
	bool visitFrame( Entity, uint, const FrameVisitor & );

	/**
	 * @brief Call a visitor with the current trajectory frame.
	 */
	bool visitCurrentFrame( Entity, const FrameVisitor & );

	/**
	 * @brief Get a copy of a frame (blocking thread), for python usage.
	 */
	Core::Struct::Frame getFrame( Entity, uint );

	/**
	 * @brief Check if a system has a multi-frame trajectory.
	 */
	bool hasMultiFrameTrajectory( Entity );

	/**
	 * @brief Get the range of currently available frames.
	 */
	App::Trajectory::FrameRange getAvailableFrames( Entity );

	/**
	 * @brief Get the trajectory data for a system.
	 */
	void get( Entity, VTX::IO::Writer::TrajectoryFrameGetter & );

} // namespace VTX::App::Helper::Trajectory

#endif
