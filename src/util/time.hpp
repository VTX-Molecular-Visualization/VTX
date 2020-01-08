#ifndef __VTX__TIME__
#define __VTX__TIME__

#ifdef _MSC_VER
#pragma once
#endif

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

			static char * getTimestamp()
			{
				std::time_t result = std::time( nullptr );
				return std::asctime( std::localtime( &result ) );
			}
		} // namespace Time
	}	  // namespace Util
} // namespace VTX

#endif
