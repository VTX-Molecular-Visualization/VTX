#ifndef __VTX_UTIL_HASHING__
#define __VTX_UTIL_HASHING__

#include <string>
#include <string_view>
#include <utility>

namespace VTX::Util::Hashing
{
	using Hash = size_t;

	template<typename T>
	inline Hash hash( const T & p_value )
	{
		return std::hash<T>()( p_value );
	}

	inline Hash hash( const std::string & p_value )
	{
		return std::hash<std::string_view>()( std::string_view( p_value ) );
	}
	inline Hash hash( const char * p_value ) { return hash( std::string( p_value ) ); }

} // namespace VTX::Util::Hashing
#endif
