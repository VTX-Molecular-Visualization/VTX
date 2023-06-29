#ifndef __VTX_CORE_GPU_MESH__
#define __VTX_CORE_GPU_MESH__

#include <util/color/rgba.hpp>
#include <util/constants.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Core::Gpu
{
	struct Mesh
	{
		Mat4f						   transform;
		std::vector<Vec3f>			   vertices;
		std::vector<Vec3f>			   normals;
		std::vector<Util::Color::Rgba> colors;
		std::vector<uint>			   visibilities;
		std::vector<uint>			   selections;
		std::vector<uint>			   ids;
		std::vector<uint>			   indices;
	};
} // namespace VTX::Core::Gpu

#endif
