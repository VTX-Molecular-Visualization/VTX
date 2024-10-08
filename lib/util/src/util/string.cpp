#include "util/string.hpp"
#include "util/types.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace VTX::Util::String
{
	inline const char * WHITESPACES = " \n\t\r\f\v";

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

		while ( endIndex != size_t( -1 ) )
		{
			splittedStr.emplace_back( p_str.substr( startIndex, endIndex - startIndex ) );
			startIndex = endIndex + 1;
			endIndex   = p_str.find( p_separator, startIndex );
		}

		splittedStr.emplace_back( p_str.substr( startIndex ) );
		splittedStr.shrink_to_fit();

		return splittedStr;
	}

	std::string & replaceAll( std::string & p_str, const std::string & p_toReplace, const std::string & p_replacement )
	{
		size_t		 currentIndex				 = p_str.find( p_toReplace, 0 );
		const size_t toReplaceLength			 = p_toReplace.length();
		const size_t indexOffsetAfterReplacement = p_replacement.length() - toReplaceLength;

		while ( currentIndex < p_str.length() )
		{
			p_str.replace( currentIndex, toReplaceLength, p_replacement );
			currentIndex += indexOffsetAfterReplacement;

			currentIndex = p_str.find( p_toReplace, currentIndex );
		}

		return p_str;
	}

	std::string floatToStr( const float p_value, const int p_nbDecimals )
	{
		std::stringstream strStream;
		strStream << std::fixed << std::setprecision( p_nbDecimals ) << p_value;
		return strStream.str();
	}

	uint strToUint( const std::string & p_str ) { return atoi( p_str.c_str() ); }

	void toUpper( std::string & p_str )
	{
		std::transform( p_str.begin(), p_str.end(), p_str.begin(), []( char & c ) { return std::toupper( c ); } );
	}

	std::string toUpper( const std::string & p_str )
	{
		std::string upcasedStr = std::string( p_str.begin(), p_str.end() );
		Util::String::toUpper( upcasedStr );

		return upcasedStr;
	}

	std::string memSizeToStr( const size_t p_size )
	{
		double			   size = static_cast<double>( p_size );
		std::ostringstream oss;

		if ( p_size < 1000 )
		{
			oss << size << " B";
		}
		else if ( p_size < 1000 * 1000 )
		{
			oss << std::fixed << std::setprecision( 2 ) << ( size / 1000 ) << " KB";
		}
		else if ( p_size < 1000 * 1000 * 1000 )
		{
			oss << std::fixed << std::setprecision( 2 ) << ( size / ( 1000 * 1000 ) ) << " MB";
		}
		else
		{
			oss << std::fixed << std::setprecision( 2 ) << ( size / ( 1000 * 1000 * 1000 ) ) << " GB";
		}

		return oss.str();
	}

} // namespace VTX::Util::String
