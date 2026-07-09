#include "util/string.hpp"
#include "util/types.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace VTX::Util::String
{
	inline const char * WHITESPACES = " \n\t\r\f\v";

	std::string trim( const std::string & p_str )
	{
		std::string result = p_str;
		result.erase( 0, result.find_first_not_of( WHITESPACES ) );
		result.erase( result.find_last_not_of( WHITESPACES ) + 1 );
		return result;
	}

	std::string trimStart( const std::string & p_str )
	{
		std::string result = p_str;
		result.erase( 0, result.find_first_not_of( WHITESPACES ) );
		return result;
	}

	std::string trimEnd( const std::string & p_str )
	{
		std::string result = p_str;
		result.erase( result.find_last_not_of( WHITESPACES ) + 1 );
		return result;
	}

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

	std::string replaceAll(
		const std::string & p_str,
		const std::string & p_toReplace,
		const std::string & p_replacement
	)
	{
		std::string	 result						 = p_str;
		size_t		 currentIndex				 = result.find( p_toReplace, 0 );
		const size_t toReplaceLength			 = p_toReplace.length();
		const size_t indexOffsetAfterReplacement = p_replacement.length() - toReplaceLength;

		while ( currentIndex < result.length() )
		{
			result.replace( currentIndex, toReplaceLength, p_replacement );
			currentIndex += indexOffsetAfterReplacement;

			currentIndex = result.find( p_toReplace, currentIndex );
		}

		return result;
	}

	std::string floatToStr( const float p_value, const int p_nbDecimals )
	{
		std::stringstream strStream;
		strStream << std::fixed << std::setprecision( p_nbDecimals ) << p_value;
		return strStream.str();
	}

	uint strToUint( const std::string & p_str ) { return atoi( p_str.c_str() ); }

	std::string toUpper( const std::string & p_str )
	{
		std::string result = p_str;

		std::transform(
			result.begin(),
			result.end(),
			result.begin(),
			[]( const unsigned char p_char ) { return static_cast<char>( std::toupper( p_char ) ); }
		);

		return result;
	}

	std::string toLower( const std::string & p_str )
	{
		std::string result = p_str;

		std::transform(
			result.begin(),
			result.end(),
			result.begin(),
			[]( const unsigned char p_char ) { return static_cast<char>( std::tolower( p_char ) ); }
		);

		return result;
	}

	std::string memSizeToStr( const size_t p_size, const bool p_isBase10 )
	{
		double			   size = static_cast<double>( p_size );
		std::ostringstream oss;

		const uint base = p_isBase10 ? 1000 : 1024;

		if ( p_size < base )
		{
			oss << size << " B";
		}
		else if ( p_size < base * base )
		{
			oss << std::fixed << std::setprecision( 2 ) << ( size / base ) << " KB";
		}
		else if ( p_size < base * base * base )
		{
			oss << std::fixed << std::setprecision( 2 ) << ( size / ( base * base ) ) << " MB";
		}
		else
		{
			oss << std::fixed << std::setprecision( 2 ) << ( size / ( base * base * base ) ) << " GB";
		}

		return oss.str();
	}

	std::string durationToStr( const float p_durationInMs )
	{
		std::ostringstream oss;
		int				   totalMilliseconds = int( p_durationInMs );

		int hours = totalMilliseconds / 3600000;
		totalMilliseconds %= 3600000;

		int minutes = totalMilliseconds / 60000;
		totalMilliseconds %= 60000;

		int seconds		 = totalMilliseconds / 1000;
		int milliseconds = totalMilliseconds % 1000;

		if ( hours > 0 )
		{
			oss << hours << " h ";
		}
		if ( minutes > 0 || hours > 0 )
		{
			oss << minutes << " min ";
		}
		if ( seconds > 0 || minutes > 0 || hours > 0 )
		{
			oss << seconds << " s ";
		}
		if ( milliseconds > 0 || ( hours == 0 && minutes == 0 && seconds == 0 ) )
		{
			oss << milliseconds << " ms";
		}

		return oss.str();
	}

} // namespace VTX::Util::String
