#ifndef __VTX_RENDERER_PROXY_VOXEL__
#define __VTX_RENDERER_PROXY_VOXEL__

namespace VTX::Renderer::Proxy
{

	struct Voxel
	{
		std::vector<Vec3f> * mins;
		std::vector<Vec3f> * maxs;
	};

} // namespace VTX::Renderer::Proxy

#endif
