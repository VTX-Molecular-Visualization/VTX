#include "logger.hpp"
#include "time.hpp"

namespace VTX
{
	namespace Util
	{
		void Logger::logDebug( std::string p_debug )
		{
#ifdef LOG_LEVEL
			if ( LOG_LEVEL > LEVEL::DEBUG ) return;
#endif
			_log( "DEBUG", p_debug );
		}

		void Logger::logInfo( std::string p_info )
		{
#ifdef LOG_LEVEL
			if ( LOG_LEVEL > LEVEL::INFO ) return;
#endif
			_log( "INFO", p_info );
		}

		void Logger::logWarning( std::string p_warning )
		{
#ifdef LOG_LEVEL
			if ( LOG_LEVEL > LEVEL::WARNING ) return;
#endif
			_log( "WARNING", p_warning );
		}

		void Logger::logError( std::string p_error )
		{
			_log( "ERROR", p_error );
		}

		void Logger::_log( std::string p_level, std::string p_string )
		{
			std::cout << "[" + Time::getNowString() + "]" + "[" + p_level + "] "
							 + p_string
					  << std::endl;
		}
	} // namespace Util
} // namespace VTX
