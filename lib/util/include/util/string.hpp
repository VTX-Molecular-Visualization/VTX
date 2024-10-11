#ifndef __VTX_UTIL_STRING__
#define __VTX_UTIL_STRING__

#include "types.hpp"
#include <algorithm>
#include <string>
#include <vector>

namespace VTX::Util::String
{
	// Trim empty chars at start and end
	void trim( std::string & p_str );
	// Trim empty chars at start
	void trimStart( std::string & p_str );
	// Trim empty chars at end
	void trimEnd( std::string & p_str );
	// Split p_str in a vector of string using char separator
	std::vector<std::string> split( const std::string & p_str, const char p_separator );
	// Replace string p_toReplace in p_str with p_replacement
	std::string & replaceAll( std::string & p_str, const std::string & p_toReplace, const std::string & p_replacement );
	// cast float to string with p_nbDecimals decimals
	std::string floatToStr( const float p_value, const int p_nbDecimals );
	// cast string to uint
	uint strToUint( const std::string & p_str );
	// uppercase all characters of p_str
	void		toUpper( std::string & p_str );
	std::string toUpper( const std::string & p_str );
	// Memory size to string.
	std::string memSizeToStr( const size_t p_size );
	// Duration to string.
	std::string durationToStr( const float p_durationInMS );

} // namespace VTX::Util::String

#endif
