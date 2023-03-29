#ifndef __VTX_UTIL_TIME__
#define __VTX_UTIL_TIME__

#ifdef _MSC_VER
#define __STDC_LIB_EXT1__ 1
#endif

#include <algorithm>
#include <chrono>
#include <ctime>
#include <string>

namespace VTX::Util::Time
{

	// inline std::chrono::steady_clock::time_point getNow() { return std::chrono::high_resolution_clock::now(); }

	inline static std::string getNowString()
	{
#ifdef __STDC_LIB_EXT1__
		__time64_t now = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
		char	   nowStr[ 26 ];
		ctime_s( nowStr, 26 * sizeof( char ), &now );
		std::string string( nowStr );
#else
		time_t		result = time( NULL );
		std::string string( ctime( &result ) );
#endif

		// Substring to remove newline.
		return string.substr( 11, string.length() - 17 );
	}

	inline static std::string getTimestamp()
	{
		std::time_t result = std::time( nullptr );
#ifdef __STDC_LIB_EXT1__
		struct tm tm;
		localtime_s( &tm, &result );
		char c[ 50 ];
		asctime_s( c, sizeof c, &tm );
		std::string str( c );
#else
		std::string str = std::asctime( std::localtime( &result ) );
#endif
		str.erase( std::remove( str.begin(), str.end(), ':' ), str.end() );
		str.erase( std::remove( str.begin(), str.end(), '\n' ), str.end() );
		return str;
	}

} // namespace VTX::Util::Time

#endif
