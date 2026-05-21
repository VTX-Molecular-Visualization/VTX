#ifndef __VTX_UTIL_ENUM__
#define __VTX_UTIL_ENUM__

#include "exceptions.hpp"
#include "logger.hpp"
#include <entt/core/enum.hpp>
#include <magic_enum/magic_enum.hpp>
#include <type_traits>

/**
 * @brief Enable bitmask operators for enum class (to add in the enum!).
 */
#define VTX_ENUM_ENABLE_BITMASK _entt_enum_as_bitmask

namespace VTX::Util::Enum
{
	/**
	 * @brief Is enum.
	 */
	template<typename T>
	concept Concept = std::is_enum_v<T>;

	/**
	 * @brief Checks if all specified bits are set in an enumeration value (must contain VTX_ENUM_ENABLE_BITMASK).
	 */
	template<Concept E>
	inline constexpr bool hasAllBits( const E p_e, const E p_bits ) noexcept
	{
		return ( p_e & p_bits ) == p_bits;
	}

	/**
	 * @brief Checks if a specific bit is set in an enumeration value (must contain VTX_ENUM_ENABLE_BITMASK).
	 */
	template<Concept E>
	inline constexpr bool hasAnyBit( const E p_e, const E p_bit ) noexcept
	{
		return ( p_e & p_bit ) != E( 0 );
	}

	/**
	 * @brief Returns the name of an enumeration value as a string view.
	 */
	template<Concept E>
	inline constexpr std::string_view enumName( const E & p_enum )
	{
		return magic_enum::enum_name( p_enum );
	}

	/**
	 * @brief Converts a string to its corresponding enumeration value of type E.
	 */
	template<Concept E>
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

	/**
	 * @brief Returns the underlying integer value of an enumeration constant.
	 */
	template<Concept E>
	inline constexpr uint enumInteger( const E & p_enum )
	{
		return magic_enum::enum_integer( p_enum );
	}

	/**
	 * @brief Returns the enumeration value of type E at the specified index.
	 */
	template<Concept E>
	inline constexpr E enumValue( const size_t p_index )
	{
		return magic_enum::enum_value<E>( p_index );
	}

	/**
	 * @brief Converts an enumeration value of one type to another enumeration type using the enumeration's name.
	 */
	template<Concept E1, Concept E2>
	inline constexpr E2 enumToAnother( const E1 & p_enum )
	{
		auto name = enumName( p_enum );
		return enumCast<E2>( std::string( name ) );
	}
} // namespace VTX::Util::Enum

#endif
