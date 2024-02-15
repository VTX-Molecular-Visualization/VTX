#ifndef __VTX_RENDERER_PROXY_VOXEL__
#define __VTX_RENDERER_PROXY_VOXEL__

namespace VTX::Renderer::Proxy
{

	struct Voxel
	{
		const std::vector<Vec3f> * mins;
		const std::vector<Vec3f> * maxs;
	};

} // namespace VTX::Renderer::Proxy

#endif
