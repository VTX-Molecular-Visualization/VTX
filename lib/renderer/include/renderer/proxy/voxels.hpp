#ifndef __VTX_RENDERER_PROXY_VOXELS__
#define __VTX_RENDERER_PROXY_VOXELS__

#include <util/types.hpp>
#include <vector>

namespace VTX::Renderer::Proxy
{

	struct Voxels
	{
		const std::vector<Vec3f> * mins;
		const std::vector<Vec3f> * maxs;
	};

} // namespace VTX::Renderer::Proxy

#endif
