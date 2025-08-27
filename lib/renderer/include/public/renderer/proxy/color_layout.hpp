#ifndef __VTX_RENDERER_PROXY_COLOR_LAYOUT__
#define __VTX_RENDERER_PROXY_COLOR_LAYOUT__

#include <core/struct/color_layout.hpp>
#include <util/callback.hpp>

namespace VTX::Renderer::Proxy
{
	/**
	 * @brief Color layout data provider.
	 */
	class ColorLayout
	{
		const Core::Struct::ColorLayout & data;

		ColorLayout( const Core::Struct::ColorLayout & p_data ) : data( p_data ) {}

		Util::Callback<>	   onChangeAll;
		Util::Callback<size_t> onChange;
	};

} // namespace VTX::Renderer::Proxy

#endif
