#ifndef __UTIL_LOGGER__
#define __UTIL_LOGGER__

#include <iostream>

#define INFO( s ) VTX::Util::Logger::LogInfo( s )
#define WARNING( s ) VTX::Util::Logger::LogWarning( s )
#define ERROR( s ) VTX::Util::Logger::LogError( s )

constexpr int LOG_LEVEL_INFO	= 0;
constexpr int LOG_LEVEL_WARNING = 1;
constexpr int LOG_LEVEL_ERROR	= 2;

namespace VTX
{
	namespace Util
	{
		class Logger
		{
		  public:
			static void LogInfo( std::string );
			static void LogWarning( std::string );
			static void LogError( std::string );

		  private:
			Logger() = default;
			static std::string FormatLog( std::string );
		};
	} // namespace Util
} // namespace VTX

#endif
