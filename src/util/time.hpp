#ifndef __VTX__TIME__
#define __VTX__TIME__

#ifdef _MSC_VER
#pragma once
#endif

#include <chrono>
#include <iostream>
#include <time.h>

namespace VTX
{
	namespace Util
	{
		namespace Time
		{
			std::string getNowString()
			{
				__time64_t now = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
				char	   nowStr[ 26 ];
				ctime_s( nowStr, 26 * sizeof( char ), &now );
				std::string string( nowStr );

				// Substring to remove newline.
				return string.substr( 11, string.length() - 17 );
			}
		} // namespace Time
	}	  // namespace Util
} // namespace VTX

#endif
