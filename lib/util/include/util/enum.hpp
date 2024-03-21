#ifndef __VTX_UTIL_ENUM__
#define __VTX_UTIL_ENUM__

// #define MAGIC_ENUM_RANGE_MIN 0
// #define MAGIC_ENUM_RANGE_MAX ( std::numeric_limits<uint16_t>::max() - 2 )

#include "concepts.hpp"
#include "exceptions.hpp"
#include "logger.hpp"
#include <magic_enum/magic_enum.hpp>
#include <type_traits>

namespace VTX::Util::Enum
{
	// Get name of an enum
	template<EnumConcept E>
	inline constexpr std::string_view enumName( const E & p_enum )
	{
		return magic_enum::enum_name( p_enum );
	}

	// Get an enum from its name
	template<EnumConcept E>
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

	template<EnumConcept E>
	inline constexpr uint enumInteger( const E & p_enum )
	{
		return magic_enum::enum_integer( p_enum );
	}

	template<EnumConcept E>
	inline constexpr E enumValue( const size_t p_index )
	{
		return magic_enum::enum_value<E>( p_index );
	}

	// Cast an enum to an other enum using it's name
	template<EnumConcept E1, EnumConcept E2>
	inline constexpr E2 enumToAnother( const E1 & p_enum )
	{
		auto name = enumName( p_enum );
		return enumCast<E2>( std::string( name ) );
	}
} // namespace VTX::Util::Enum

#endif
