#include "logger.hpp"

namespace VTX
{
	namespace Util
	{
		void Logger::LogInfo( std::string p_info )
		{
#ifdef LOG_LEVEL
			if ( LOG_LEVEL > LOG_LEVEL_INFO ) return;
#endif
			std::cout << FormatLog( p_info ) << std::endl;
		}

		void Logger::LogWarning( std::string p_warning )
		{
#ifdef LOG_LEVEL
			if ( LOG_LEVEL > LOG_LEVEL_WARNING ) return;
#endif
			std::cout << FormatLog( p_warning ) << std::endl;
		}

		void Logger::LogError( std::string p_error )
		{
			std::cerr << FormatLog( p_error ) << std::endl;
		}

		std::string Logger::FormatLog( std::string p_string )
		{
			return std::string( p_string );
		}
	} // namespace Util
} // namespace VTX
