#ifndef __VTX_UTIL_HASHING__
#define __VTX_UTIL_HASHING__

#include "types.hpp"
#include <string>
#include <string_view>
#include <utility>

namespace VTX::Util
{
	// Template function to hash a value
	template<typename T>
	inline Hash hash( const T & p_value )
	{
		return std::hash<T>()( p_value );
	}

	// Speicalization for std::string, using content instead of ptr
	inline Hash hash( const std::string & p_value )
	{
		return std::hash<std::string_view>()( std::string_view( p_value ) );
	}
	// Speicalization for char*, using content instead of ptr
	inline Hash hash( const char * p_value ) { return hash( std::string( p_value ) ); }

	template<typename T>
	inline Hash hash()
	{
		return typeid( std::remove_pointer_t<std::decay_t<T>> ).hash_code();
	}

} // namespace VTX::Util
#endif
