#ifndef __VTX_RENDERER_PROXY_REPRESENTATIONS__
#define __VTX_RENDERER_PROXY_REPRESENTATIONS__

#include <util/types.hpp>

namespace VTX::Renderer::Proxy
{

	struct Representation
	{
		bool  hasShphere		= true;
		float radiusSphereFixed = 0.4f;
		float radiusSphereAdd	= 0.f;
		bool  radiusFixed		= true;

		bool  hasCylinder	 = true;
		float radiusCylinder = 0.1f;
		uint  cylinderColorBlendingMode;

		bool hasRibbon = true;
		uint ribbonColorBlendingMode;

		bool  hasSes;
		float sesResolution;
	};

	using Representations = const std::vector<Representation>;

} // namespace VTX::Renderer::Proxy

#endif