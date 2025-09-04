#ifndef __VTX_CORE_STRUCT_COLOR_LAYOUT__
#define __VTX_CORE_STRUCT_COLOR_LAYOUT__

#include <array>
#include <util/color/rgba.hpp>

namespace VTX::Core::Struct
{
	constexpr size_t COLOR_LAYOUT_SIZE = 256;
	using ColorLayoutArray			   = std::array<Util::Color::Rgba, COLOR_LAYOUT_SIZE>;

	struct ColorLayout
	{
		ColorLayoutArray colors;
	};
} // namespace VTX::Core::Struct

#endif
