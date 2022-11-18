#ifndef __VTX_TOOL_LOGGER__
#define __VTX_TOOL_LOGGER__

#include "manager/event/event.hpp"
#include <iostream>
#include <string>
#include <vector>

// TODO VTXEventLog moved here. Include it in VTX_UTIL
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
			// IO::Writer::Log _writer = IO::Writer::Log( Util::Time::getTimestamp() );

			Logger()							 = default;
			Logger( const Logger & )			 = delete;
			Logger & operator=( const Logger & ) = delete;
			~Logger()							 = default;
		};
	} // namespace Tool

	namespace Event
	{
		// Other events.
		struct VTXEventLog : public VTXEvent
		{
			VTXEventLog( const VTX_EVENT &	 p_event,
						 const std::string & p_level,
						 const std::string & p_date,
						 const std::string & p_message ) :
				VTXEvent( p_event ),
				level( p_level ), date( p_date ), message( p_message )
			{
			}
			std::string level;
			std::string date;
			std::string message;
		};
	} // namespace Event

	// TODO VTX_LOGIC Managed in VTX_UTILS
	template<typename... T>
	inline void VTX_DEBUG( const std::string p_str, T &&... p_args )
	{
		// Tool::Logger::get().logDebug( fmt::format( p_str, p_args... ) );
	}
	template<typename... T>
	inline void VTX_INFO( const std::string p_str, T &&... p_args )
	{
		// Tool::Logger::get().logInfo( fmt::format( p_str, p_args... ) );
	}
	template<typename... T>
	inline void VTX_WARNING( const std::string p_str, T &&... p_args )
	{
		// Tool::Logger::get().logWarning( fmt::format( p_str, p_args... ) );
	}
	template<typename... T>
	inline void VTX_ERROR( const std::string p_str, T &&... p_args )
	{
		// Tool::Logger::get().logError( fmt::format( p_str, p_args... ) );
	}
	template<typename... T>
	inline void VTX_CONSOLE( const std::string p_str, T &&... p_args )
	{
		// std::cout << fmt::format( p_str, p_args... ) << std::endl;
	}
	template<typename... T>
	inline void VTX_LOG_FILE( const std::string & p_str, T &&... p_args )
	{
		// Tool::Logger::get().logInFile( fmt::format( p_str, p_args... ) );
	}
	template<typename... T>
	inline void VTX_LOG( const Tool::Logger::LEVEL & p_level, const std::string & p_str, T &&... p_args )
	{
		// Tool::Logger::get().log( p_level, fmt::format( p_str, p_args... ) );
	}

} // namespace VTX

#endif
