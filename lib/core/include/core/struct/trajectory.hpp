#ifndef __VTX_CORE_STRUCT_TRAJECTORY__
#define __VTX_CORE_STRUCT_TRAJECTORY__

#include <cstddef>
#include <span>
#include <util/types.hpp>
#include <vector>

namespace VTX::Core::Struct
{
	/**
	 * @brief A frame is a set of atom positions.
	 */
	using Frame = std::vector<Vec3f>;

	/**
	 * @brief Frame view, not owning.
	 */
	using FrameView = std::span<const Vec3f>;

	/**
	 * @brief Trajectory.
	 */
	struct Trajectory
	{
		/**
		 * @brief Frame count.
		 */
		size_t frameCount = 1;

		/**
		 * @brief Storage.
		 */
		std::vector<Frame> frames;
	};

} // namespace VTX::Core::Struct

#endif
