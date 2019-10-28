#ifndef __LOGGER__
#define __LOGGER__

#include <iostream>

#define DBG( s ) VTX::Util::Logger::logInfo( s )
#define INF( s ) VTX::Util::Logger::logInfo( s )
#define WRG( s ) VTX::Util::Logger::logWarning( s )
#define ERR( s ) VTX::Util::Logger::logError( s )

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

			static void logDebug( const std::string );
			static void logInfo( const std::string );
			static void logWarning( const std::string );
			static void logError( const std::string );

		  private:
			Logger() = default;
			static void _log( const std::string, const std::string );
		};
	} // namespace Util
} // namespace VTX

#endif
