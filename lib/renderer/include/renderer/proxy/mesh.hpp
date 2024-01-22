#ifndef __VTX_RENDERER_PROXY_MESH__
#define __VTX_RENDERER_PROXY_MESH__

#include <util/types.hpp>
#include <vector>

namespace VTX::Renderer::Proxy
{
	struct Mesh
	{
		Mat4f *				 transform;
		std::vector<Vec3f> * vertices;
		std::vector<Vec3f> * normales;
		std::vector<uchar> * colors;
		std::vector<bool> *	 visibilities;
		std::vector<bool> *	 selections;
		std::vector<uint> *	 ids;
		std::vector<uint> *	 indices;
	};

} // namespace VTX::Renderer::Proxy

#endif
