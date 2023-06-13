#ifndef __VTX_CORE_NEW_STRUCT_TRAJECTORY__
#define __VTX_CORE_NEW_STRUCT_TRAJECTORY__

#include <util/types.hpp>
#include <vector>

namespace VTX::Core::NewStruct
{
	using Frame = std::vector<Vec3f>;
	class Trajectory
	{
	  public:
		std::vector<Frame> frames;
		size_t			   currentFrameIndex = 0;
	};

} // namespace VTX::Core::NewStruct

#endif
