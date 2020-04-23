#ifndef __VTX_UTIL_LOGGER__
#define __VTX_UTIL_LOGGER__

#ifdef _MSC_VER
#pragma once
#endif

#include <string>

namespace VTX
{
	namespace Util
	{
		namespace Logger
		{
			enum class LEVEL
			{
				LOG_LVL_DEBUG,
				LOG_LVL_INFO,
				LOG_LVL_WARNING,
				LOG_LVL_ERROR
			};

			void log( const LEVEL, const std::string & );

			inline void logDebug( const std::string & p_debug ) { log( LEVEL::LOG_LVL_DEBUG, p_debug ); }
			inline void logInfo( const std::string & p_info ) { log( LEVEL::LOG_LVL_INFO, p_info ); }
			inline void logWarning( const std::string & p_warning ) { log( LEVEL::LOG_LVL_WARNING, p_warning ); }
			inline void logError( const std::string & p_error ) { log( LEVEL::LOG_LVL_ERROR, p_error ); }

		}; // namespace Logger
	}	   // namespace Util

	inline void VTX_DEBUG( const std::string & p_str ) { VTX::Util::Logger::logDebug( p_str ); }
	inline void VTX_INFO( const std::string & p_str ) { VTX::Util::Logger::logInfo( p_str ); }
	inline void VTX_WARNING( const std::string & p_str ) { VTX::Util::Logger::logWarning( p_str ); }
	inline void VTX_ERROR( const std::string & p_str ) { VTX::Util::Logger::logError( p_str ); }
} // namespace VTX

#endif
