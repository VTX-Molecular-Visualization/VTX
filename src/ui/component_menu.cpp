#include "component_menu.hpp"
#include "../defines.hpp"
#include "../lib/imgui/imgui.h"
#include "../localization/language.hpp"
#include "../util/logger.hpp"
#include "../vtx_app.hpp"

namespace VTX
{
	namespace UI
	{
		void ComponentMenu::_draw()
		{
			if ( ImGui::BeginMainMenuBar() )
			{
				if ( ImGui::BeginMenu( LOCALE( "MainMenu.Menu" ) ) )
				{
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Menu.Quit" ) ) )
					{ VTXApp::INSTANCE().stop(); }

					ImGui::EndMenu();
				}

				ImGui::EndMainMenuBar();
			}
		}
	} // namespace UI
} // namespace VTX
