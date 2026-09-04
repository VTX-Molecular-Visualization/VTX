#ifndef __VTX_APP_TRAJECTORY_TYPES__
#define __VTX_APP_TRAJECTORY_TYPES__

#include <core/struct/trajectory.hpp>
#include <cstdint>
#include <util/math/range.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Trajectory
{
	/**
	 * @brief Types.
	 */
	using FrameRange = Util::Math::Range<Index>;

	/**
	 * @brief Trajectory modes.
	 */
	enum struct TRAJECTORY_BUFFER_MODE : uint8_t
	{
		// Load all frames.
		FULL,
		// Load a frames window.
		CIRCULAR
	};

	/**
	 * @brief Trajectory buffer filling direction.
	 */
	enum struct TRAJECTORY_READ_DIRECTION : uint8_t
	{
		FORWARD,
		BACKWARD
	};

	/**
	 * @brief Loaded frame.
	 */
	struct LoadedFrame
	{
		uint				index;
		Core::Struct::Frame positions;
	};

	/**
	 * @brief Loaded frames batch.
	 * Used to post data between threads.
	 */
	struct LoadedFrameBatch
	{
		FrameRange				 availableFrames;
		std::vector<LoadedFrame> frames;
		uint64_t				 requestVersion = 0;
	};

} // namespace VTX::App::Trajectory

#endif
