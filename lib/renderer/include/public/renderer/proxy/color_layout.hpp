#ifndef __VTX_RENDERER_PROXY_COLOR_LAYOUT__
#define __VTX_RENDERER_PROXY_COLOR_LAYOUT__

#include <core/struct/color_layout.hpp>
#include <util/callback.hpp>

namespace VTX::Renderer::Proxy
{
	/**
	 * @brief Color layout data provider.
	 */
	struct ColorLayout
	{
		const Core::Struct::ColorLayout & layout;

		ColorLayout( const Core::Struct::ColorLayout & p_layout ) : layout( p_layout ) {}

		Util::Callback<>	   onChangeAll;
		Util::Callback<size_t> onChange;
	};

} // namespace VTX::Renderer::Proxy

#endif
