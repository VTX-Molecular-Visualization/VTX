#include "component_menu.hpp"
#include "../defines.hpp"
#include "../lib/imgui/imgui.h"
#include "../localization/language.hpp"
#include "../setting.hpp"
#include "../style.hpp"
#include "../util/logger.hpp"
#include "../vtx_app.hpp"

namespace VTX
{
	namespace UI
	{
		ComponentMenu::ComponentMenu( bool * const p_show, bool * const p_showConsole, bool * const p_showScene ) :
			BaseComponent( p_show ), _showConsole( p_showConsole ), _showScene( p_showScene )
		{
		}

		void ComponentMenu::_addComponents()
		{ /*_addComponent( new ComponentDialogImport( &_showDialogImport ) );*/
		}

		void ComponentMenu::_draw()
		{
			ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding,
								 ImVec2( IMGUI_STYLE_MENUBAR_PADDING, IMGUI_STYLE_MENUBAR_PADDING ) );
			if ( ImGui::BeginMainMenuBar() )
			{
				if ( ImGui::BeginMenu( LOCALE( "MainMenu.Menu" ), _show ) )
				{
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Menu.Quit" ) ) ) { VTXApp::get().stop(); }

					ImGui::EndMenu();
				}

				/*
				if ( ImGui::BeginMenu( LOCALE( "MainMenu.Import" ), _show ) )
				{
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Import.MMTF" ) ) ) {}

					ImGui::EndMenu();
				}
				*/

				if ( ImGui::BeginMenu( LOCALE( "MainMenu.Display" ), _show ) )
				{
					ImGui::Checkbox( LOCALE( "MainMenu.Display.Scene" ), _showScene );
					ImGui::Checkbox( LOCALE( "MainMenu.Display.Console" ), _showConsole );

					ImGui::EndMenu();
				}

				if ( ImGui::BeginMenu( LOCALE( "MainMenu.Settings" ), _show ) )
				{
					int mode = Setting::Rendering::mode;
					if ( ImGui::Combo( "Rendering", &mode, "Forward\0Deferred\0" ) )
					{
						switch ( mode )
						{
						case 0: Setting::Rendering::mode = Renderer ::MODE ::FORWARD; break;
						case 1: Setting::Rendering::mode = Renderer ::MODE ::DEFERRED; break;
						default: break;
						}
					}

					ImGui::Separator();
					ImGui::Checkbox( LOCALE( "MainMenu.Settings.AA" ), &Setting::Rendering::useAA );
					ImGui::Checkbox( LOCALE( "MainMenu.Settings.SSAO" ), &Setting::Rendering::useSSAO );
					ImGui::Checkbox( LOCALE( "MainMenu.Settings.BLUR" ), &Setting::Rendering::useBlur );

					ImGui::EndMenu();
				}

				ImGui::PopStyleVar();
				ImGui::EndMainMenuBar();
			}
		}
	} // namespace UI
} // namespace VTX
