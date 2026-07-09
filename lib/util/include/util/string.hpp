#ifndef __VTX_UTIL_STRING__
#define __VTX_UTIL_STRING__

#include "types.hpp"
#include <algorithm>
#include <string>
#include <vector>

namespace VTX::Util::String
{
	/**
	 * Trim empty characters at the start and end and return the result.
	 */
	[[nodiscard]] std::string trim( const std::string & p_str );

	/**
	 * Trim empty characters at the start and return the result.
	 */
	[[nodiscard]] std::string trimStart( const std::string & p_str );

	/**
	 * Trim empty characters at the end and return the result.
	 */
	[[nodiscard]] std::string trimEnd( const std::string & p_str );

	/**
	 * Split a string using a character separator.
	 */
	[[nodiscard]] std::vector<std::string> split( const std::string & p_str, const char p_separator );

	/**
	 * Replace all occurrences of a string and return the result.
	 */
	[[nodiscard]] std::string replaceAll(
		const std::string & p_str,
		const std::string & p_toReplace,
		const std::string & p_replacement
	);

	/**
	 * Convert a float to a string with a fixed number of decimals.
	 */
	[[nodiscard]] std::string floatToStr( const float p_value, const int p_nbDecimals );

	/**
	 * Convert a string to an unsigned integer.
	 */
	[[nodiscard]] uint strToUint( const std::string & p_str );

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
	[[nodiscard]] std::string memSizeToStr( const size_t p_size, const bool p_isBase10 = true );

	/**
	 * Convert a duration to a string.
	 */
	[[nodiscard]] std::string durationToStr( const float p_durationInMS );

} // namespace VTX::Util::String

#endif
