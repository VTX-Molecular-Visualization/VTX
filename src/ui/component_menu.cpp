#include "component_menu.hpp"
#include "../defines.hpp"
#include "../lib/imgui/imgui.h"
#include "../util/logger.hpp"

namespace VTX
{
	namespace UI
	{
		void ComponentMenu::_draw()
		{
			// Menu Bar
			if ( ImGui::BeginMainMenuBar() )
			{
				ImGui::SetWindowFontScale( 1.5f );
				if ( ImGui::BeginMenu( "Menu" ) )
				{
					if ( ImGui::MenuItem( "Quit" ) ) {}

					ImGui::EndMenu();
				}

				ImGui::EndMainMenuBar();
			}
		}
	} // namespace UI
} // namespace VTX
