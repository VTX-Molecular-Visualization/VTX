#ifndef __VTX_CORE_STRUCT_MESH__
#define __VTX_CORE_STRUCT_MESH__

#include <util/color/rgba.hpp>
#include <util/constants.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Core
{
	struct StructMesh
	{
		Mat4f						   tranform;
		std::vector<Vec3f>			   vertices;
		std::vector<Vec3f>			   normals;
		std::vector<Util::Color::Rgba> colors;
		std::vector<uint>			   visibilities;
		std::vector<uint>			   selections;
		std::vector<uint>			   ids;
		std::vector<uint>			   indices;
	};
} // namespace VTX::Core

#endif
