#include "component_console.hpp"
#include "../defines.hpp"
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

		void ComponentConsole::_draw()
		{
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
			ImGui::SetNextWindowDockID( ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE ),
										ImGuiCond_FirstUseEver );

			if ( ImGui::Begin( LOCALE( "Console.Console" ), _show, flags )
				 == false )
			{
				ImGui::End();
				return;
			}

			ImGui::End();
		}

		void ComponentConsole::_clear() {}
	} // namespace UI
} // namespace VTX
