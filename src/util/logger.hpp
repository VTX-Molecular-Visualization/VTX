#ifndef __VTX_LOGGER__
#define __VTX_LOGGER__

#include <iostream>

#define VTX_DEBUG( s ) VTX::Util::Logger::logInfo( s )
#define VTX_INFO( s ) VTX::Util::Logger::logInfo( s )
#define VTX_WARNING( s ) VTX::Util::Logger::logWarning( s )
#define VTX_ERROR( s ) VTX::Util::Logger::logError( s )

namespace VTX
{
	namespace Util
	{
		class Logger
		{
		  public:
			enum class LEVEL
			{
				LOG_LVL_DEBUG,
				LOG_LVL_INFO,
				LOG_LVL_WARNING,
				LOG_LVL_ERROR
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
