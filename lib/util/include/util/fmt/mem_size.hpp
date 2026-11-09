#ifndef VTX_UTIL_FMT_MEM_SIZE_HPP
#define VTX_UTIL_FMT_MEM_SIZE_HPP

#include "util/string.hpp"
#include <cstddef>
#include <fmt/format.h>
#include <string_view>

template<>
struct fmt::formatter<VTX::Util::String::FormattedMemSize> : fmt::formatter<std::string_view>
{
	auto format( const VTX::Util::String::FormattedMemSize & p_memSize, format_context & p_ctx ) const
	{
		const double	  size = static_cast<double>( p_memSize.size );
		const std::size_t base = p_memSize.isBase10 ? 1000 : 1024;

		if ( p_memSize.size < base )
		{
			return fmt::format_to( p_ctx.out(), "{} B", size );
		}
		if ( p_memSize.size < base * base )
		{
			return fmt::format_to( p_ctx.out(), "{:.2f} KB", size / static_cast<double>( base ) );
		}
		if ( p_memSize.size < base * base * base )
		{
			return fmt::format_to( p_ctx.out(), "{:.2f} MB", size / static_cast<double>( base * base ) );
		}

		return fmt::format_to( p_ctx.out(), "{:.2f} GB", size / static_cast<double>( base * base * base ) );
	}
};

#endif
