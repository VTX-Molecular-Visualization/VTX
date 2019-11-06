#ifndef __VTX_LOGGER__
#define __VTX_LOGGER__

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
			enum class LEVEL
			{
				DEBUG = 0,
				INFO,
				WARNING,
				ERROR
			};

			struct Log
			{
				LEVEL		level;
				std::string date;
				std::string message;
			};

			static void logDebug( const std::string & );
			static void logInfo( const std::string & );
			static void logWarning( const std::string & );
			static void logError( const std::string & );

		  private:
			Logger() = default;
			static void _log( const LEVEL, const std::string & );
		};
	} // namespace Util
} // namespace VTX

#endif
