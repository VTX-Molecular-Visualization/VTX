#ifndef __VTX_CORE_STRUCT_TRAJECTORY__
#define __VTX_CORE_STRUCT_TRAJECTORY__

#include <util/types.hpp>
#include <vector>

namespace VTX::Core::Struct
{
	using Frame = std::vector<Vec3f>;
	class Trajectory
	{
	  public:
		void fillFrame( const size_t p_moleculeFrameIndex, const std::vector<Vec3f> & p_atomPositions )
		{
			Frame & frame = frames[ p_moleculeFrameIndex ];
			frame.resize( p_atomPositions.size() );

			std::copy( p_atomPositions.begin(), p_atomPositions.end(), frame.begin() );
		}

		std::vector<Frame> frames;
		size_t			   currentFrameIndex = 0;
	};

} // namespace VTX::Core::Struct

#endif
