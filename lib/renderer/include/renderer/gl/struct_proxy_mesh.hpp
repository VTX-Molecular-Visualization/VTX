#ifndef __VTX_RENDERER_GL_STRUCT_PROXY_MESH__
#define __VTX_RENDERER_GL_STRUCT_PROXY_MESH__

#include <util/color/rgba.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Renderer::GL
{
	struct StructProxyMesh
	{
		Mat4f *							 transform;
		std::vector<Vec3f> *			 vertices;
		std::vector<Vec3f> *			 normals;
		std::vector<Util::Color::Rgba> * colors;
		std::vector<size_t> *			 visibilities;
		std::vector<size_t> *			 selections;
		std::vector<size_t> *			 ids;
		std::vector<size_t> *			 indices;
	};

} // namespace VTX::Renderer::GL

#endif
