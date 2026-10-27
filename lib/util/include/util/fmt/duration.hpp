#ifndef VTX_UTIL_FMT_DURATION_HPP
#define VTX_UTIL_FMT_DURATION_HPP

#include "util/string.hpp"
#include <fmt/format.h>
#include <string_view>

template<>
struct fmt::formatter<VTX::Util::String::FormattedDuration> : fmt::formatter<std::string_view>
{
	auto format( const VTX::Util::String::FormattedDuration & p_duration, format_context & p_ctx ) const
	{
		int totalMilliseconds = static_cast<int>( p_duration.durationInMs );

		const int hours = totalMilliseconds / 3600000;
		totalMilliseconds %= 3600000;

		const int minutes = totalMilliseconds / 60000;
		totalMilliseconds %= 60000;

		const int seconds	   = totalMilliseconds / 1000;
		const int milliseconds = totalMilliseconds % 1000;

		auto out = p_ctx.out();
		if ( hours > 0 )
		{
			out = fmt::format_to( out, "{} h ", hours );
		}
		if ( minutes > 0 || hours > 0 )
		{
			out = fmt::format_to( out, "{} min ", minutes );
		}
		if ( seconds > 0 || minutes > 0 || hours > 0 )
		{
			out = fmt::format_to( out, "{} s ", seconds );
		}
		if ( milliseconds > 0 || ( hours == 0 && minutes == 0 && seconds == 0 ) )
		{
			out = fmt::format_to( out, "{} ms", milliseconds );
		}

		return out;
	}
};

#endif
