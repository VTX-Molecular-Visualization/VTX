#ifndef __VTX_CORE_STRUCT_TRAJECTORY__
#define __VTX_CORE_STRUCT_TRAJECTORY__

#include <util/types.hpp>
#include <vector>

namespace VTX::Core::Struct
{
	/**
	 * @brief Each index is correlated with other vector's that contains atom's data
	 */
	using Frame = std::vector<Vec3f>;
	struct Trajectory
	{
		void fillFrame( const size_t p_moleculeFrameIndex, const std::vector<Vec3f> & p_atomPositions )
		{
			Frame & frame = frames[ p_moleculeFrameIndex ];
			frame.resize( p_atomPositions.size() );

			std::copy( p_atomPositions.begin(), p_atomPositions.end(), frame.begin() );
		}

		const Frame & getCurrentFrame() const { return frames[ currentFrameIndex ]; }
		Frame &		  getCurrentFrame() { return frames[ currentFrameIndex ]; }

		size_t getFrameCount() const { return frames.size(); }

		std::vector<Frame> frames;
		size_t			   currentFrameIndex = 0;
	};

} // namespace VTX::Core::Struct

#endif
