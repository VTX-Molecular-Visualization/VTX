#ifndef __UTIL_LOGGER__
#define __UTIL_LOGGER__

#include <iostream>

#define DEBUG( s ) VTX::Util::Logger::logInfo( s )
#define INFO( s ) VTX::Util::Logger::logInfo( s )
#define WARNING( s ) VTX::Util::Logger::logWarning( s )
#define ERROR( s ) VTX::Util::Logger::logError( s )

constexpr int LOG_LEVEL_DEBUG	= 0;
constexpr int LOG_LEVEL_INFO	= 1;
constexpr int LOG_LEVEL_WARNING = 2;
constexpr int LOG_LEVEL_ERROR	= 3;

namespace VTX
{
	namespace Util
	{
		class Logger
		{
		  public:
			static void logDebug( std::string );
			static void logInfo( std::string );
			static void logWarning( std::string );
			static void logError( std::string );

		  private:
			Logger() = default;
			static std::string _formatLog( std::string );
		};
	} // namespace Util
} // namespace VTX

#endif
