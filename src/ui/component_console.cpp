#include "component_console.hpp"
#include "../defines.hpp"
#include "../event/event.hpp"
#include "../lib/imgui/imgui.h"
#include "../localization/language.hpp"
#include "../style.hpp"
#include "../util/logger.hpp"

namespace VTX
{
	namespace UI
	{
		ComponentConsole::ComponentConsole( bool * const p_show ) : BaseComponent( p_show ) {}

		void ComponentConsole::_registerEventHandlers() { _registerEventHandler( VTX::Event::EVENT_UI::LOG_CONSOLE ); }

		void ComponentConsole::_applyEvent( const Event::EVENT_UI p_event, void * p_arg )
		{
			if ( p_event == Event::EVENT_UI::LOG_CONSOLE )
			{
				Util::Logger::Log * log = (Util::Logger::Log *)( p_arg );
				_addLog( *log );
			}
		}

		void ComponentConsole::_draw()
		{
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
			ImGui::SetNextWindowDockID( ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE ), ImGuiCond_FirstUseEver );

			if ( ImGui::Begin( LOCALE( "Console.Console" ), _show, flags ) == false )
			{
				ImGui::End();
				return;
			}
			for ( Util::Logger::Log log : _logs )
			{
				ImGui::Text( ( "[" + log.date + "] " + "[" + log.level + "] " + log.message ).c_str() );
			}

			if ( ImGui::GetScrollY() >= ImGui::GetScrollMaxY() ) { ImGui::SetScrollHereY( 1.0f ); }

			ImGui::End();
		}
		void ComponentConsole::_addLog( const Util::Logger::Log & p_log )
		{
			_logs.push_back( p_log );
			if ( _logs.size() > VTX_CONSOLE_SIZE ) { _logs.pop_front(); }
		}

		void ComponentConsole::_clear() { _logs.clear(); }
	} // namespace UI
} // namespace VTX
