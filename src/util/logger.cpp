#include "logger.hpp"
#include "time.hpp"
#include <algorithm>

namespace VTX
{
	namespace Util
	{
		void Logger::logDebug( const std::string p_debug )
		{
#ifdef LOG_LEVEL
			if ( LOG_LEVEL > LEVEL::DEBUG ) return;
#endif
			_log( "DEBUG", p_debug );
		}

		void Logger::logInfo( const std::string p_info )
		{
#ifdef LOG_LEVEL
			if ( LOG_LEVEL > LEVEL::INFO ) return;
#endif
			_log( "INFO", p_info );
		}

		void Logger::logWarning( const std::string p_warning )
		{
#ifdef LOG_LEVEL
			if ( LOG_LEVEL > LEVEL::WARNING ) return;
#endif
			_log( "WARNING", p_warning );
		}

		void Logger::logError( const std::string p_error )
		{
			_log( "ERROR", p_error );
		}

		void Logger::_log( const std::string p_level,
						   const std::string p_string )
		{
			std::string str( p_string );
			str.erase( std::remove( str.begin(), str.end(), '\n' ), str.end() );

			std::cout << "[" + Time::getNowString() + "]" + "[" + p_level + "] "
							 + str
					  << std::endl;
		}
	} // namespace Util
} // namespace VTX
