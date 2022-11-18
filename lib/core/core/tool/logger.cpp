#include "logger.hpp"
#include "manager/event/event_manager.hpp"
#include <algorithm>

#define LOG_TO_FILE

namespace VTX
{
	namespace Tool
	{
		void Logger::log( const LEVEL & p_level, const std::string & p_message )
		{
			//			std::string level( magic_enum::enum_name( p_level ).substr( 8 ) );
			//			std::string date( Util::Time::getNowString() );
			//			std::string message( p_message );
			//			message.erase( std::remove( message.begin(), message.end(), '\n' ), message.end() );
			//			std::string messageToWrite = "[" + date + "] " + "[" + level + "] " + message;
			//
			// #ifdef VTX_PRODUCTION
			//			if ( p_level == LEVEL::LOG_LVL_DEBUG )
			//				return;
			// #endif
			//
			//			VTX_EVENT( new Event::VTXEventLog( Event::Global::LOG_CONSOLE, level, date, message ) );
			//
			// #ifdef LOG_LEVEL
			//			if ( LOG_LEVEL > p_level )
			//				return;
			// #endif
			//
			// #ifdef LOG_TO_FILE
			//			logInFile( messageToWrite );
			// #endif
			//
			//			std::cout << messageToWrite << std::endl;
		}

		void Logger::logInFile( const std::string & p_message )
		{
			//_writer.writeFile( IO::FilePath(), p_message );
		}

	} // namespace Tool
} // namespace VTX
