#ifndef __VTX_UTIL_ENUM__
#define __VTX_UTIL_ENUM__

#include "exceptions.hpp"
#include <magic_enum.hpp>

namespace VTX::Util::Enum
{

	template<typename T>
	inline constexpr std::string_view enumName( T p_enum )
	{
		return magic_enum::enum_name( p_enum );
	}

	template<typename T>
	inline constexpr T enumCast( const std::string & p_name )
	{
		auto value = magic_enum::enum_cast<T>( p_name );

		if ( value.has_value() )
		{
			return value.value();
		}
		else
		{
			throw LibException( "Invalid enum name: " + p_name );
		}
	}

} // namespace VTX::Util::Enum

#endif
