#ifndef __VTX_UTIL_STRING__
#define __VTX_UTIL_STRING__

#include <algorithm>
#include <string>

namespace VTX::Util::String
{
	static const char * WHITESPACES = " \n\t\r\f\v";

	void trim( std::string & p_str );
	void trimStart( std::string & p_str );
	void trimEnd( std::string & p_str );

} // namespace VTX::Util::String

#endif