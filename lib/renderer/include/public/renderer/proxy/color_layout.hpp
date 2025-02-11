#ifndef __VTX_RENDERER_PROXY_COLOR_LAYOUT__
#define __VTX_RENDERER_PROXY_COLOR_LAYOUT__

#include <array>
#include <util/callback.hpp>
#include <util/color/rgba.hpp>

namespace VTX::Renderer::Proxy
{
	/**
	 * @brief Color layout data provider.
	 */
	struct ColorLayout
	{
		const std::vector<Util::Color::Rgba> * colors;

		Util::Callback<>	   onChangeAll;
		Util::Callback<size_t> onChange;
	};

} // namespace VTX::Renderer::Proxy

#endif
