#ifndef __VTX_UTIL_FMT_RANGE__
#define __VTX_UTIL_FMT_RANGE__

#include "util/math/range.hpp"
#include <fmt/format.h>

template<std::integral T>
struct fmt::formatter<VTX::Util::Math::Range<T>> : fmt::formatter<T>
{
	auto format( const VTX::Util::Math::Range<T> & p_range, format_context & p_ctx ) const
	{
		auto out = std::copy_n( "[ ", 2, p_ctx.out() );

		out = formatter<T>::format( p_range.getFirst(), p_ctx );

		out = std::copy_n( " - ", 3, out );
		p_ctx.advance_to( out );

		out = formatter<T>::format( p_range.getLast(), p_ctx );

		return std::copy_n( " ]", 2, out );
	}
};

#endif
