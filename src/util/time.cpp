#include "time.hpp"
#include <chrono>
#include <time.h>

namespace VTX
{
	namespace Util
	{
		std::string Time::getNowString()
		{
			__time64_t now = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
			char	   nowStr[ 26 ];
			ctime_s( nowStr, 26 * sizeof( char ), &now );
			std::string coucou( nowStr );

			// Substring to remove newline.
			return coucou.substr( 11, coucou.length() - 17 );
		}
	} // namespace Util
} // namespace VTX
