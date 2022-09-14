#include "util/string.hpp"
#include <iomanip>
#include <sstream>

namespace VTX::Util::String
{
	void trim( std::string & p_str )
	{
		trimStart( p_str );
		trimEnd( p_str );
	}
	void trimStart( std::string & p_str ) { p_str.erase( 0, p_str.find_first_not_of( WHITESPACES ) ); }
	void trimEnd( std::string & p_str ) { p_str.erase( p_str.find_last_not_of( WHITESPACES ) + 1 ); }

	std::vector<std::string> split( const std::string & p_str, const char p_separator )
	{
		std::vector<std::string> splittedStr = std::vector<std::string>();

		size_t startIndex = 0;
		size_t endIndex	  = p_str.find( p_separator );

		while ( endIndex != -1 )
		{
			splittedStr.emplace_back( p_str.substr( startIndex, endIndex - startIndex ) );
			startIndex = endIndex + 1;
			endIndex   = p_str.find( p_separator, startIndex );
		}

		splittedStr.emplace_back( p_str.substr( startIndex ) );
		splittedStr.shrink_to_fit();

		return splittedStr;
	}

	std::string floatToStr( const float p_value, const int p_nbDecimals )
	{
		std::stringstream strStream;
		strStream << std::fixed << std::setprecision( p_nbDecimals ) << p_value;
		return strStream.str();
	}

	uint strToUint( const std::string & p_str ) { return atoi( p_str.c_str() ); }

} // namespace VTX::Util::String
