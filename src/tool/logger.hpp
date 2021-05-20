#ifndef __VTX_TOOL_LOGGER__
#define __VTX_TOOL_LOGGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "io/writer/log.hpp"
#include "util/time.hpp"
#include <iostream>
#include <string>

namespace VTX
{
	namespace Tool
	{
		class Logger final
		{
		  public:
			enum class LEVEL
			{
				LOG_LVL_DEBUG,
				LOG_LVL_INFO,
				LOG_LVL_WARNING,
				LOG_LVL_ERROR,
			};

			inline static const std::vector<std::string> LEVEL_STR { "DEBUG", "INFO", "WARNING", "ERROR" };

			inline static Logger & get()
			{
				static Logger instance;
				return instance;
			}

			inline void logDebug( const std::string & p_debug ) { log( LEVEL::LOG_LVL_DEBUG, p_debug ); }
			inline void logInfo( const std::string & p_info ) { log( LEVEL::LOG_LVL_INFO, p_info ); }
			inline void logWarning( const std::string & p_warning ) { log( LEVEL::LOG_LVL_WARNING, p_warning ); }
			inline void logError( const std::string & p_error ) { log( LEVEL::LOG_LVL_ERROR, p_error ); }

			void log( const LEVEL &, const std::string & );
			void logInFile( const std::string & );

		  private:
			IO::Writer::Log _writer = IO::Writer::Log( Util::Time::getTimestamp() );

			Logger()				 = default;
			Logger( const Logger & ) = delete;
			Logger & operator=( const Logger & ) = delete;
			~Logger()							 = default;
		};
	} // namespace Tool

	inline void VTX_DEBUG( const std::string & p_str ) { Tool::Logger::get().logDebug( p_str ); }
	inline void VTX_INFO( const std::string & p_str ) { Tool::Logger::get().logInfo( p_str ); }
	inline void VTX_WARNING( const std::string & p_str ) { Tool::Logger::get().logWarning( p_str ); }
	inline void VTX_ERROR( const std::string & p_str ) { Tool::Logger::get().logError( p_str ); }
	inline void VTX_CONSOLE( const std::string & p_str ) { std::cout << p_str << std::endl; }
	inline void VTX_LOG_FILE( const std::string & p_str ) { Tool::Logger::get().logInFile( p_str ); }
	inline void VTX_LOG( const Tool::Logger::LEVEL & p_level, const std::string & p_str )
	{
		Tool::Logger::get().log( p_level, p_str );
	}
} // namespace VTX

#endif
