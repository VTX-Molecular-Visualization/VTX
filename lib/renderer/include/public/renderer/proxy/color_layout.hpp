#ifndef __VTX_RENDERER_PROXY_COLOR_LAYOUT__
#define __VTX_RENDERER_PROXY_COLOR_LAYOUT__

#include "renderer/color.hpp"
#include <util/callback.hpp>

namespace VTX::Renderer::Proxy
{
	/**
	 * @brief Color layout data provider.
	 */
	struct ColorLayout
	{
		const Color::Layout & layout;

		ColorLayout( const Color::Layout & p_layout ) : layout( p_layout ) {}

		Util::Callback<>	   onChangeAll;
		Util::Callback<size_t> onChange;
	};

} // namespace VTX::Renderer::Proxy

#endif
