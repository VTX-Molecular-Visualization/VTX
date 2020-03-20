#ifndef __VTX_TIME__
#define __VTX_TIME__

#ifdef _MSC_VER
#pragma once
#endif

#define __STDC_LIB_EXT1__ 1

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iostream>
#include <time.h>

namespace VTX
{
	namespace Util
	{
		namespace Time
		{
			static __time64_t getNow()
			{
				return std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
			}

			static std::string getNowString()
			{
				__time64_t now = getNow();
				char	   nowStr[ 26 ];
				ctime_s( nowStr, 26 * sizeof( char ), &now );
				std::string string( nowStr );

				// Substring to remove newline.
				return string.substr( 11, string.length() - 17 );
			}

			static std::string getTimestamp()
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
		} // namespace Time
	}	  // namespace Util
} // namespace VTX

#endif
