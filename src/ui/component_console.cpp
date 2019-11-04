#include "component_console.hpp"
#include "../defines.hpp"
#include "../lib/imgui/imgui.h"
#include "../localization/language.hpp"
#include "../style.hpp"

namespace VTX
{
	namespace UI
	{
		ComponentConsole::ComponentConsole( bool * p_show ) :
			BaseComponent( p_show )
		{
		}

		void ComponentConsole::_registerEventHandlers()
		{
			_registerEventHandler( Event::EVENT_UI::LOG_CONSOLE,
								   []( va_list p_args ) {

								   } );
		}

		void ComponentConsole::_draw()
		{
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
			ImGui::SetNextWindowDockID( ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE ),
										ImGuiCond_FirstUseEver );

			if ( ImGui::Begin( LOCALE( "Console.Console" ), _show, flags )
				 == false )
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

		void ComponentConsole::addLog( const Util::Logger::Log p_log )
		{
			_logs.push_back( p_log );
		}

		void ComponentConsole::_clear() { _logs.clear(); }
	} // namespace UI
} // namespace VTX
