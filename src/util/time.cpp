#include "time.hpp"
#include <chrono>
#include <ctime>

namespace VTX
{
	namespace Util
	{
		std::string Time::getNowString()
		{
			__time64_t	now = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
			std::string nowStr( ctime( &now ) );

			// Substring to remove newline.
			return nowStr.substr( 11, nowStr.length() - 17 );
		}
	} // namespace Util
} // namespace VTX
