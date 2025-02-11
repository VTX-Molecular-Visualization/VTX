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
	inline Hash hash( const char * p_value ) { return std::hash<std::string_view>()( std::string_view( p_value ) ); }

	template<typename T>
	using DefaultHashProcess = std::remove_pointer_t<std::decay_t<T>>;

	template<typename T>
	constexpr Hash hash()
	{
		return typeid( DefaultHashProcess<T> ).hash_code();
	}

	template<typename T>
	constexpr std::string_view typeName()
	{
		return typeid( DefaultHashProcess<T> ).name();
	}

} // namespace VTX::Util
#endif
