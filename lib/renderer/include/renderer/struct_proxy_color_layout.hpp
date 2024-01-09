#ifndef __VTX_RENDERER_STRUCT_PROXY_COLKLOR_LAYOUT__
#define __VTX_RENDERER_STRUCT_PROXY_COLKLOR_LAYOUT__

#include <array>
#include <util/color/rgba.hpp>

namespace VTX::Renderer
{
	struct StructProxyColorLayout
	{
		std::array<Util::Color::Rgba, 256> * colors;
	};

} // namespace VTX::Renderer

#endif
