#ifndef __VTX_RENDERER_PROXY_MESH__
#define __VTX_RENDERER_PROXY_MESH__

#include <util/types.hpp>
#include <vector>

namespace VTX::Renderer::Proxy
{
	/**
	 * @brief Mesh data provider.
	 */
	struct Mesh
	{
		const Mat4f *			   transform;
		const std::vector<Vec3f> * vertices;
		const std::vector<Vec3f> * normales;
		const std::vector<uchar> * colors;
		const std::vector<bool> *  visibilities;
		const std::vector<bool> *  selections;
		const std::vector<uint> *  ids;
		const std::vector<uint> *  indices;
	};

} // namespace VTX::Renderer::Proxy

#endif
