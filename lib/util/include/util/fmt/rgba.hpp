#ifndef __VTX_UTIL_FMT_COLOR_RGBA__
#define __VTX_UTIL_FMT_COLOR_RGBA__

#include "util/color/rgba.hpp"
#include "util/fmt/glm.hpp"
#include "util/types.hpp"

// This file contains implementations of fmt::formatter to allow logging of RGBA color

// Color::Rgba inherit from glm::vec4f
template<>
struct fmt::formatter<VTX::Util::Color::Rgba> : fmt::formatter<VTX::Vec4f>
{
	auto format( const VTX::Util::Color::Rgba & p_color, fmt::format_context & p_ctx ) const
	{
		return formatter<VTX::Vec4f>::format( p_color, p_ctx );
	}
};

#endif
