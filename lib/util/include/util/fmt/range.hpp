#ifndef __VTX_UTIL_FMT_RANGE__
#define __VTX_UTIL_FMT_RANGE__

#include "util/math/range.hpp"
#include <fmt/format.h>
#include <optional>

// This file contains implementations of fmt::formatter to allow logging of Util::Math::Range

template<std::integral T>
struct fmt::formatter<VTX::Util::Math::Range<T>> : fmt::formatter<T>
{
	auto format( const VTX::Util::Math::Range<T> & p_range, format_context & p_ctx ) const
	{
		auto out = std::copy_n( "[ ", 2, p_ctx.out() );

		out = formatter<T>::format( p_range.first, p_ctx );

		out = std::copy_n( " - ", 3, out );
		p_ctx.advance_to( out );

		out = formatter<T>::format( p_range.last, p_ctx );

		return std::copy_n( " ]", 2, out );
	}
};

#endif
