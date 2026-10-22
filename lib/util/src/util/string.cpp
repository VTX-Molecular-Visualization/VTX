#include "util/string.hpp"
#include "util/types.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace VTX::Util::String
{

	std::string floatToStr( const float p_value, const int p_nbDecimals )
	{
		std::stringstream strStream;
		strStream << std::fixed << std::setprecision( p_nbDecimals ) << p_value;
		return strStream.str();
	}

	std::string toUpper( const std::string & p_str )
	{
		std::string result = p_str;

		std::ranges::transform(
			result,
			result.begin(),
			[]( const unsigned char p_char ) { return static_cast<char>( std::toupper( p_char ) ); }
		);

		return result;
	}

	std::string toLower( const std::string & p_str )
	{
		std::string result = p_str;

		std::ranges::transform(
			result,
			result.begin(),
			[]( const unsigned char p_char ) { return static_cast<char>( std::tolower( p_char ) ); }
		);

		return result;
	}

	std::string memSizeToStr( const size_t p_size, const bool p_isBase10 )
	{
		const double	   size = static_cast<double>( p_size );
		std::ostringstream oss;

		const uint base = p_isBase10 ? 1000 : 1024;

		if ( p_size < base )
		{
			oss << size << " B";
		}
		else if ( p_size < static_cast<size_t>( base ) * base )
		{
			oss << std::fixed << std::setprecision( 2 ) << ( size / base ) << " KB";
		}
		else if ( p_size < static_cast<size_t>( base ) * base * base )
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
		int				   totalMilliseconds = static_cast<int>( p_durationInMs );

		const int hours = totalMilliseconds / 3600000;
		totalMilliseconds %= 3600000;

		const int minutes = totalMilliseconds / 60000;
		totalMilliseconds %= 60000;

		const int seconds	   = totalMilliseconds / 1000;
		const int milliseconds = totalMilliseconds % 1000;

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
