#ifndef __VTX_RENDERER_STRUCT_PROXY_MESH__
#define __VTX_RENDERER_STRUCT_PROXY_MESH__

#include <util/color/rgba.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Renderer
{
	struct StructProxyMesh
	{
		Mat4f *							 transform;
		std::vector<Vec3f> *			 vertices;
		std::vector<Vec3f> *			 normals;
		std::vector<Util::Color::Rgba> * colors;
		std::vector<uchar> *			 visibilities;
		std::vector<uchar> *			 selections;
		std::vector<uint> *				 ids;
		std::vector<uint> *				 indices;
	};

} // namespace VTX::Renderer

#endif
