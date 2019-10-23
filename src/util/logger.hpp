#ifndef __UTIL_LOGGER__
#define __UTIL_LOGGER__

#include <iostream>

#define DEBUG( s ) VTX::Util::Logger::logInfo( s )
#define INFO( s ) VTX::Util::Logger::logInfo( s )
#define WARNING( s ) VTX::Util::Logger::logWarning( s )
#define ERROR( s ) VTX::Util::Logger::logError( s )

namespace VTX
{
	namespace Util
	{
		class Logger
		{
		  public:
			enum LEVE
			{
				DEBUG = 0,
				INFO,
				WARNING,
				ERROR
			};

			static void logDebug( std::string );
			static void logInfo( std::string );
			static void logWarning( std::string );
			static void logError( std::string );

		  private:
			Logger() = default;
			static void _log( std::string, std::string );
		};
	} // namespace Util
} // namespace VTX

#endif
