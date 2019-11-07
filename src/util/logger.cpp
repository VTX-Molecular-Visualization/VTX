#include "logger.hpp"
#include "../event/event.hpp"
#include "../vtx_app.hpp"
#include "time.hpp"
#include <algorithm>

namespace VTX
{
	namespace Util
	{
		void Logger::logDebug( const std::string & p_debug )
		{
			_log( LEVEL::LOG_LVL_DEBUG, p_debug );
		}

		void Logger::logInfo( const std::string & p_info )
		{
			_log( LEVEL::LOG_LVL_INFO, p_info );
		}

		void Logger::logWarning( const std::string & p_warning )
		{
			_log( LEVEL::LOG_LVL_WARNING, p_warning );
		}

		void Logger::logError( const std::string & p_error )
		{
			_log( LEVEL::LOG_LVL_ERROR, p_error );
		}

		void Logger::_log( const LEVEL p_level, const std::string & p_message )
		{
			Log log;
			log.level = p_level;
			std::string message( p_message );
			message.erase( std::remove( message.begin(), message.end(), '\n' ),
						   message.end() );
			log.message = message;
			log.date	= Time::getNowString();

			if ( VTXApp::isRunning() )
			{
				/*
				VTXApp::get().fireUIEvent(
					Event::EVENT_UI::LOG_CONSOLE,  );
					*/
			}

#ifdef LOG_LEVEL
			if ( LOG_LEVEL > p_level ) return;
#endif

			std::cout << "[" + log.date + "] " //+ "[" + p_level + "] "
							 + log.message
					  << std::endl;
		}
	} // namespace Util
} // namespace VTX
