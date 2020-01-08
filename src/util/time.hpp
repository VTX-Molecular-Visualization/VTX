#ifndef __VTX__TIME__
#define __VTX__TIME__

#ifdef _MSC_VER
#pragma once
#endif

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
				std::string str	   = std::asctime( std::localtime( &result ) );
				str.erase( std::remove( str.begin(), str.end(), ':' ), str.end() );
				str.erase( std::remove( str.begin(), str.end(), '\n' ), str.end() );
				return str;
			}
		} // namespace Time
	}	  // namespace Util
} // namespace VTX

#endif
