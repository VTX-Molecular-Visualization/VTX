#ifndef __VTX_UTIL_STRING__
#define __VTX_UTIL_STRING__

#include "types.hpp"
#include <algorithm>
#include <charconv>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <vector>

namespace VTX::Util::String
{
	/**
	 * @brief Whitespace chars.
	 */
	constexpr std::string_view WHITESPACES = " \n\t\r\f\v";

	/**
	 * Trim empty characters at the start and end.
	 */
	[[nodiscard]] constexpr std::string trim( const std::string & p_str )
	{
		std::string result = p_str;
		result.erase( 0, result.find_first_not_of( WHITESPACES ) );
		result.erase( result.find_last_not_of( WHITESPACES ) + 1 );
		return result;
	}

	/**
	 * @brief Replace first occurrence.
	 */
	[[nodiscard]] constexpr std::string replaceFirst(
		const std::string & p_str,
		const std::string & p_toReplace,
		const std::string & p_replacement
	)
	{
		std::string result = p_str;
		result.replace( result.find( p_toReplace ), p_toReplace.size(), p_replacement );
		return result;
	}

	/**
	 * Trim empty characters at the start and return the result.
	 */
	[[nodiscard]] constexpr std::string trimStart( const std::string & p_str )
	{
		std::string result = p_str;
		result.erase( 0, result.find_first_not_of( WHITESPACES ) );
		return result;
	}

	/**
	 * Trim empty characters at the end and return the result.
	 */
	[[nodiscard]] constexpr std::string trimEnd( const std::string & p_str )
	{
		std::string result = p_str;
		result.erase( result.find_last_not_of( WHITESPACES ) + 1 );
		return result;
	}

	/**
	 * Split a string using a character separator.
	 */
	[[nodiscard]] constexpr std::vector<std::string> split( const std::string & p_str, const char p_separator )
	{
		std::vector<std::string> splittedStr = std::vector<std::string>();

		size_t startIndex = 0;
		size_t endIndex	  = p_str.find( p_separator );

		while ( endIndex != std::string::npos )
		{
			splittedStr.emplace_back( p_str.substr( startIndex, endIndex - startIndex ) );
			startIndex = endIndex + 1;
			endIndex   = p_str.find( p_separator, startIndex );
		}

		splittedStr.emplace_back( p_str.substr( startIndex ) );
		splittedStr.shrink_to_fit();

		return splittedStr;
	}

	/**
	 * Replace all occurrences of a string and return the result.
	 */
	[[nodiscard]] constexpr std::string replaceAll(
		const std::string & p_str,
		const std::string & p_toReplace,
		const std::string & p_replacement
	)
	{
		std::string result = p_str;
		if ( p_toReplace.empty() )
		{
			return result;
		}
		size_t currentIndex = result.find( p_toReplace );

		while ( currentIndex != std::string::npos )
		{
			result.replace( currentIndex, p_toReplace.size(), p_replacement );
			currentIndex += p_replacement.size();

			currentIndex = result.find( p_toReplace, currentIndex );
		}

		return result;
	}

	/**
	 * Convert a float to a string with a fixed number of decimals.
	 */
	[[nodiscard]] std::string floatToStr( float p_value, int p_nbDecimals );

	/**
	 * Convert a string to an unsigned integer.
	 */
	template<typename T>
	[[nodiscard]] T strToNumber( const std::string & p_str )
	{
		static_assert( std::is_integral_v<T> );

		T value {};

		const auto [ ptr, ec ] = std::from_chars( p_str.data(), p_str.data() + p_str.size(), value );

		if ( ec != std::errc {} || ptr != p_str.data() + p_str.size() )
		{
			throw std::invalid_argument( "Invalid numeric value: " + p_str );
		}

		return value;
	}

	/**
	 * Convert all characters to uppercase and return the result.
	 */
	[[nodiscard]] std::string toUpper( const std::string & p_str );

	/**
	 * Convert all characters to lowercase and return the result.
	 */
	[[nodiscard]] std::string toLower( const std::string & p_str );

	/**
	 * Convert a memory size to a string.
	 */
	[[nodiscard]] std::string memSizeToStr( size_t p_size, bool p_isBase10 = true );

	/**
	 * Convert a duration to a string.
	 */
	[[nodiscard]] std::string durationToStr( float p_durationInMS );

} // namespace VTX::Util::String

#endif
