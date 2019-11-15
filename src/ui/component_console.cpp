#include "component_console.hpp"
#include "../defines.hpp"
#include "../event/event.hpp"
#include "../lib/imgui/imgui.h"
#include "../localization/language.hpp"
#include "../style.hpp"

namespace VTX
{
	namespace UI
	{
		ComponentConsole::ComponentConsole( bool * const p_show ) : BaseComponent( p_show ) {}

		void ComponentConsole::_registerEventHandlers() { _registerEventHandler( VTX::Event::EVENT_UI::LOG_CONSOLE ); }

		void ComponentConsole::_applyEvent( const Event::EVENT_UI p_event, const int p_args... )
		{
			if ( p_event == Event::EVENT_UI::LOG_CONSOLE )
			{ //_addLog( p_args[ 1 ] );
			}
		}

		void ComponentConsole::_draw()
		{
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
			ImGui::SetNextWindowDockID( ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE ), ImGuiCond_FirstUseEver );

			if ( ImGui::Begin( LOCALE( "Console.Console" ), _show, flags ) == false )
			{
				ImGui::ListBoxHeader( "" );
				for ( Util::Logger::Log log : _logs )
				{
					ImGui::Text( log.message.c_str() );
				}
				ImGui::ListBoxFooter();

				ImGui::End();
				return;
			}

			ImGui::End();
		}
		void ComponentConsole::_addLog( const Util::Logger::Log & p_log ) { _logs.push_back( p_log ); }

		void ComponentConsole::_clear() { _logs.clear(); }
	} // namespace UI
} // namespace VTX
