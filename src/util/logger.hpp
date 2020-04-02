#ifndef __VTX_LOGGER__
#define __VTX_LOGGER__

#ifdef _MSC_VER
#pragma once
#endif

#include <iostream>

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

			static void logDebug( const std::string & );
			static void logInfo( const std::string & );
			static void logWarning( const std::string & );
			static void logError( const std::string & );

		  private:
			Logger() = default;
			static void _log( const LEVEL, const std::string & );
		};
	} // namespace Util

	inline void VTX_DEBUG( const std::string & p_str ) { VTX::Util::Logger::logDebug( p_str ); }
	inline void VTX_INFO( const std::string & p_str ) { VTX::Util::Logger::logInfo( p_str ); }
	inline void VTX_WARNING( const std::string & p_str ) { VTX::Util::Logger::logWarning( p_str ); }
	inline void VTX_ERROR( const std::string & p_str ) { VTX::Util::Logger::logError( p_str ); }
} // namespace VTX

#endif
