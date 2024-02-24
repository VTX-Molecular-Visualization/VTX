#ifndef __VTX_RENDERER_PROXY_REPRESENTATIONS__
#define __VTX_RENDERER_PROXY_REPRESENTATIONS__

#include <util/types.hpp>

namespace VTX::Renderer::Proxy
{

	struct Representation
	{
		bool  hasShphere;
		float radiusSphere;

		bool  hasCylinder;
		float radiusCylinder;

		bool  hasRibbon;
		uchar ribbonColorBlendingMode;

		bool  hasSes;
		float sesResolution;
	};

	using Representations = const std::array<Representation, 256>;

} // namespace VTX::Renderer::Proxy

#endif
