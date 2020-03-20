#include "logger.hpp"
#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "time.hpp"
#include "vtx_app.hpp"
#include <algorithm>
#include <magic_enum.hpp>

namespace VTX
{
	namespace Util
	{
		void Logger::logDebug( const std::string & p_debug ) { _log( LEVEL::LOG_LVL_DEBUG, p_debug ); }

		void Logger::logInfo( const std::string & p_info ) { _log( LEVEL::LOG_LVL_INFO, p_info ); }

		void Logger::logWarning( const std::string & p_warning ) { _log( LEVEL::LOG_LVL_WARNING, p_warning ); }

		void Logger::logError( const std::string & p_error ) { _log( LEVEL::LOG_LVL_ERROR, p_error ); }

		void Logger::_log( const LEVEL p_level, const std::string & p_message )
		{
			std::string level( magic_enum::enum_name( p_level ).substr( 8 ) );
			std::string date( Time::getNowString() );
			std::string message( p_message );
			message.erase( std::remove( message.begin(), message.end(), '\n' ), message.end() );

#ifndef _DEBUG
			if ( p_level == LEVEL::LOG_LVL_DEBUG ) return;
#endif

			if ( VTXApp::isRunning() )
			{ VTX_EVENT( new Event::VTXEventLog( Event::Global::LOG_CONSOLE, level, date, message ) ); }

#ifdef LOG_LEVEL
			if ( LOG_LEVEL > p_level ) return;
#endif
			std::cout << "[" + date + "] " + "[" + level + "] " + message << std::endl;
		}
	} // namespace Util
} // namespace VTX
