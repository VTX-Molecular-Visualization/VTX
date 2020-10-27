#include "logger.hpp"
#include "event/event_manager.hpp"
#include <algorithm>
#include <magic_enum.hpp>

#define LOG_TO_FILE

namespace VTX
{
	namespace Tool
	{
		void Logger::_log( const LEVEL p_level, const std::string & p_message )
		{
			std::string level( magic_enum::enum_name( p_level ).substr( 8 ) );
			std::string date( Util::Time::getNowString() );
			std::string message( p_message );
			message.erase( std::remove( message.begin(), message.end(), '\n' ), message.end() );
			std::string messageToWrite = "[" + date + "] " + "[" + level + "] " + message;

#ifndef _DEBUG
			if ( p_level == LEVEL::LOG_LVL_DEBUG )
				return;
#endif

			VTX_EVENT( new Event::VTXEventLog( Event::Global::LOG_CONSOLE, level, date, message ) );

#ifdef LOG_LEVEL
			if ( LOG_LEVEL > p_level )
				return;
#endif

#ifdef LOG_TO_FILE
			_writer.writeFile( "", messageToWrite );
#endif

			std::cout << messageToWrite << std::endl;
		}

	} // namespace Tool
} // namespace VTX
