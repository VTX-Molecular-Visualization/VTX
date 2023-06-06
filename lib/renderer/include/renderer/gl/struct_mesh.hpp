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

	static const StructMesh DEFAULT_MESH = StructMesh {
		MAT4F_ID,
		std::vector<Vec3f> { Vec3f( 0.5f, -0.5f, 0.f ), Vec3f( -0.5f, -0.5f, 0.f ), Vec3f( 0.f, 0.5f, 0.f ) },
		std::vector<Vec3f> { Vec3f( 0.f, 0.f, 1.f ), Vec3f( 0.f, 0.f, 1.f ), Vec3f( 0.f, 0.f, 1.f ) },
		std::vector<Util::Color::Rgba> { Util::Color::Rgba( 1.f, 0.f, 0.f, 1.f ),
										 Util::Color::Rgba( 0.f, 1.f, 0.f, 1.f ),
										 Util::Color::Rgba( 0.f, 0.f, 1.f, 1.f ) },
		std::vector<uint> { 1, 1, 1, 1 },
		std::vector<uint> { 0, 0, 0, 0 },
		std::vector<uint> { 0, 0, 0, 0 },
		std::vector<uint> { 0, 1, 2 }
	};

} // namespace VTX::Core

#endif
