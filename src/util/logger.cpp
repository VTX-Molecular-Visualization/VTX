#include "logger.hpp"
#include "../event/event_ui.hpp"
#include "../vtx_app.hpp"
#include "time.hpp"
#include <algorithm>

namespace VTX
{
	namespace Util
	{
		void Logger::logDebug( const std::string & p_debug )
		{
			_log( LEVEL::DEBUG, p_debug );
		}

		void Logger::logInfo( const std::string & p_info )
		{
			_log( LEVEL::INFO, p_info );
		}

		void Logger::logWarning( const std::string & p_warning )
		{
			_log( LEVEL::WARNING, p_warning );
		}

		void Logger::logError( const std::string & p_error )
		{
			_log( LEVEL::ERROR, p_error );
		}

		void Logger::_log( const LEVEL p_level, const std::string & p_string )
		{
			if ( VTXApp::isRunning() )
			{
				VTXApp::INSTANCE().fireUIEvent(
					Event::EVENT_UI::LOG_CONSOLE, p_level, p_string );
			}

#ifdef LOG_LEVEL
			if ( LOG_LEVEL > p_level ) return;
#endif

			std::string str( p_string );
			str.erase( std::remove( str.begin(), str.end(), '\n' ), str.end() );

			std::cout << "[" + Time::getNowString()
							 + "] " //+ "[" + p_level + "] "
							 + str
					  << std::endl;
		}
	} // namespace Util
} // namespace VTX
