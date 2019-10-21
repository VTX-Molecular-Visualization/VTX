#include "logger.hpp"
#include "time.hpp"

namespace VTX
{
	namespace Util
	{
		void Logger::logDebug( std::string p_debug )
		{
#ifdef LOG_LEVEL
			if ( LOG_LEVEL > LOG_LEVEL_DEBUG ) return;
#endif
			std::cout << _formatLog( p_debug ) << std::endl;
		}

		void Logger::logInfo( std::string p_info )
		{
#ifdef LOG_LEVEL
			if ( LOG_LEVEL > LOG_LEVEL_INFO ) return;
#endif
			std::cout << _formatLog( p_info ) << std::endl;
		}

		void Logger::logWarning( std::string p_warning )
		{
#ifdef LOG_LEVEL
			if ( LOG_LEVEL > LOG_LEVEL_WARNING ) return;
#endif
			std::cout << _formatLog( p_warning ) << std::endl;
		}

		void Logger::logError( std::string p_error )
		{
			std::cerr << _formatLog( p_error ) << std::endl;
		}

		std::string Logger::_formatLog( std::string p_string )
		{
			return "[" + Time::getNowString() + "] " + p_string;
		}
	} // namespace Util
} // namespace VTX
