#ifndef __VTX_CORE_STRUCT_COLOR_LAYOUT__
#define __VTX_CORE_STRUCT_COLOR_LAYOUT__

#include <array>
#include <string>
#include <util/callback.hpp>
#include <util/color/rgba.hpp>

namespace VTX::Core::Struct
{
	class ColorLayout
	{
	  public:
		static inline const size_t LAYOUT_SIZE = 256;

		ColorLayout();
		ColorLayout( const std::vector<Util::Color::Rgba> & p_layout );

		std::vector<Util::Color::Rgba> layout;
	};
} // namespace VTX::Core::Struct

#endif
