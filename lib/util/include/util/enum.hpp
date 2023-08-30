#ifndef __VTX_UTIL_ENUM__
#define __VTX_UTIL_ENUM__

#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX ( std::numeric_limits<uint16_t>::max() - 2 )

#include "exceptions.hpp"
#include <magic_enum.hpp>
#include <type_traits>

namespace VTX::Util::Enum
{

	template<typename E>
	inline constexpr std::string_view enumName( const E & p_enum )
	{
		return magic_enum::enum_name( p_enum );
	}

	template<typename E>
	inline constexpr E enumCast( const std::string & p_name )
	{
		auto value = magic_enum::enum_cast<E>( p_name );

		if ( value.has_value() )
		{
			return value.value();
		}
		else
		{
			throw LibException( "Invalid enum name: " + p_name );
		}
	}

	template<typename E1, typename E2>
	inline constexpr E2 enumToAnother( const E1 & p_enum )
	{
		return enumCast<E2>( std::string( enumName( p_enum ) ) );
	}

	// 	template<typename E>
	// 	auto toInt( E const value ) -> typename std::underlying_type<E>::type
	// 	{
	// 		return static_cast<typename std::underlying_type<E>::type>( value );
	// 	}

	template<typename E>
	inline constexpr int toInt( const E & p_enum )
	{
		return magic_enum::enum_integer( p_enum ); // or magic_enum::enum_underlying(color);
	}

} // namespace VTX::Util::Enum

#endif
