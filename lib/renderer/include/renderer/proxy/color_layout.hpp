#ifndef __VTX_RENDERER_PROXY_COLOR_LAYOUT__
#define __VTX_RENDERER_PROXY_COLOR_LAYOUT__

#include <array>
#include <util/color/rgba.hpp>

namespace VTX::Renderer::Proxy
{

	struct ColorLayout
	{
		const std::vector<Util::Color::Rgba> * colors;

		Util::Callback<> onChange;
	};

} // namespace VTX::Renderer::Proxy

#endif
