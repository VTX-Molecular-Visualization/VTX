#ifndef __VTX_APP_HELPER_TRAJECTORY__
#define __VTX_APP_HELPER_TRAJECTORY__

#include "app/ecs.hpp"
#include "app/system/trajectory.hpp"
#include <span>
#include <util/math/range.hpp>

namespace VTX::IO::Writer
{
	class TrajectoryFrameGetter;
}

namespace VTX::App::Helper::Trajectory
{
	/**
	 * @brief Range of frames available in a trajectory.
	 */
	using FrameRange = Util::Math::Range<Index>;

	/**
	 * @brief View of a frame (no ownership).
	 */
	using FrameView = std::span<const Vec3f>;

	/**
	 * @brief Get the current atom positions of a system.
	 */
	FrameView getCurrentAtomPositions( const Entity & );

	/**
	 * @brief Get the atom positions of a system at a specific frame.
	 */
	FrameView getAtomPositions( const Entity &, const uint );

	/**
	 * @brief Get the number of frames available.
	 */
	uint getFrameCount( const Entity & );

	/**
	 * @brief Get the current frame index.
	 */
	uint getCurrentFrameIndex( const Entity & ) noexcept;

	/**
	 * @brief Check if a specific frame is available.
	 */
	bool isFrameAvailable( const Entity &, const uint );

	/**
	 * @brief Check if a system has a trajectory.
	 */
	bool hasTrajectory( const Entity & ) noexcept;

	/**
	 * @brief Check if a system has a multi-frame trajectory.
	 */
	bool hasMultiFrameTrajectory( const Entity & ) noexcept;

	/**
	 * @brief Get the range of available frames.
	 */
	FrameRange getAvailableFrames( const Entity & );

	/**
	 * @brief Get the trajectory data for a system.
	 */
	void get( const Entity &, VTX::IO::Writer::TrajectoryFrameGetter & );

	/**
	 * @brief Get the generic trajectory data for a system.
	 */
	const VTX::App::System::GenericTrajectory * getGeneric( const Entity & ) noexcept;
} // namespace VTX::App::Helper::Trajectory

#endif
