#ifndef __VTX_UTIL_STRING__
#define __VTX_UTIL_STRING__

#include <util/types.hpp>
#include <algorithm>
#include <string>
#include <vector>

namespace VTX::Util::String
{
	static const char * WHITESPACES = " \n\t\r\f\v";

	void trim( std::string & p_str );
	void trimStart( std::string & p_str );
	void trimEnd( std::string & p_str );

	std::vector<std::string> split( const std::string & p_str, const char p_separator );
	std::string & replaceAll( std::string & p_str, const std::string & p_toReplace, const std::string & p_replacement );

	std::string floatToStr( const float p_value, const int p_nbDecimals );
	uint		strToUint( const std::string & p_str );
} // namespace VTX::Util::String

#endif
