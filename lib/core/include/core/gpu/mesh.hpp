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
		std::vector<size_t>			   visibilities;
		std::vector<size_t>			   selections;
		std::vector<size_t>			   ids;
		std::vector<size_t>			   indices;
	};
} // namespace VTX::Core::Gpu

#endif
