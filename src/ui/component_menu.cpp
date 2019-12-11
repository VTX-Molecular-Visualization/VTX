#include "component_menu.hpp"
#include "../defines.hpp"
#include "../style.hpp"
#include "../util/logger.hpp"
#include "../vtx_app.hpp"

namespace VTX
{
	namespace UI
	{
		ComponentMenu::ComponentMenu( bool * const p_show,
									  bool * const p_showConsole,
									  bool * const p_showScene,
									  bool * const p_showSelection ) :
			BaseComponent( p_show ),
			_showConsole( p_showConsole ), _showScene( p_showScene ), _showSelection( p_showSelection )
		{
		}

		void ComponentMenu::_addComponents() {}

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

				if ( ImGui::BeginMenu( LOCALE( "MainMenu.Display" ), _show ) )
				{
					ImGui::Checkbox( LOCALE( "MainMenu.Display.Scene" ), _showScene );
					ImGui::Checkbox( LOCALE( "MainMenu.Display.Selection" ), _showSelection );
					ImGui::Checkbox( LOCALE( "MainMenu.Display.Console" ), _showConsole );
					ImGui::EndMenu();
				}

				if ( ImGui::BeginMenu( LOCALE( "MainMenu.Settings" ), _show ) )
				{
					if ( ImGui::Combo( LOCALE( "MainMenu.Settings.Theme" ),
									   (int *)&Setting::UI::theme,
									   "Light\0Dark\0Classic\0" ) )
					{ VTXApp::get().setTheme(); }

					ImGui::Separator();

					if ( ImGui::Combo( LOCALE( "MainMenu.Settings.SymbolDisplay" ),
									   (int *)&Setting::UI::symbolDisplayMode,
									   "Short\0Long\0" ) )
					{}

					ImGui::Separator();

					/*
					if ( ImGui::Combo(
							 LOCALE( "MainMenu.Settings.Render" ), (int *)&Setting::Rendering::mode, "Deferred\0" ) )
					{}
					*/
					if ( ImGui::Combo( LOCALE( "MainMenu.Settings.Representation" ),
									   (int *)&Setting::Rendering::reprensation,
									   "Ball and stick\0Van der Waals\0" ) )
					{ VTXApp::get().getScene().setupMoleculeViews(); }

					if ( ImGui::Combo( LOCALE( "MainMenu.Settings.Shading" ),
									   (int *)&Setting::Rendering::shading,
									   "Lambert\0Blinn Phong\0Toon\0" ) )
					{ VTXApp::get().setRendererShading(); }

					ImGui::Separator();
					// ImGui::Checkbox( LOCALE( "MainMenu.Settings.AA" ), &Setting::Rendering::useAA );
					ImGui::Checkbox( LOCALE( "MainMenu.Settings.SSAO" ), &Setting::Rendering::useSSAO );

					ImGui::EndMenu();
				}

				ImGui::PopStyleVar();
				ImGui::EndMainMenuBar();
			}
		}
	} // namespace UI
} // namespace VTX
