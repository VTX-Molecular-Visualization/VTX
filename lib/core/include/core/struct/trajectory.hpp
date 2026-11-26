#ifndef VTX_CORE_STRUCT_TRAJECTORY_HPP
#define VTX_CORE_STRUCT_TRAJECTORY_HPP

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

		/**
		 * @brief Get atom count.
		 */
		size_t getAtomCount() const noexcept
		{
			if ( frames.empty() )
			{
				return 0;
			}

			return frames[ 0 ].size();
		}
	};

} // namespace VTX::Core::Struct

#endif
