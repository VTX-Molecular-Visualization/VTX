#ifndef __VTX_RENDERER_PROXY_COLOR_LAYOUT__
#define __VTX_RENDERER_PROXY_COLOR_LAYOUT__

#include <array>
#include <util/color/rgba.hpp>

namespace VTX::Renderer::Proxy
{
	using ColorLayout  = const std::array<Util::Color::Rgba, 256>;
	using ColorLayouts = std::vector<ColorLayout>;

} // namespace VTX::Renderer::Proxy

#endif
