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
									  bool * const p_showInspector ) :
			BaseComponent( p_show ),
			_showConsole( p_showConsole ), _showScene( p_showScene ), _showInspector( p_showInspector )
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
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Menu.New" ) ) ) { VTXApp::get().getScene().clear(); }
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Menu.Quit" ) ) ) { VTXApp::get().stop(); }

					ImGui::EndMenu();
				}

				if ( ImGui::BeginMenu( LOCALE( "MainMenu.Display" ), _show ) )
				{
					ImGui::Checkbox( LOCALE( "MainMenu.Display.Scene" ), _showScene );
					ImGui::Checkbox( LOCALE( "MainMenu.Display.Inspector" ), _showInspector );
					ImGui::Checkbox( LOCALE( "MainMenu.Display.Console" ), _showConsole );
					ImGui::Separator();
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Display.CloseAll" ) ) )
					{
						*_showScene		= false;
						*_showInspector = false;
						*_showConsole	= false;
					}
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Display.ShowAll" ) ) )
					{
						*_showScene		= true;
						*_showInspector = true;
						*_showConsole	= true;
					}
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
									   (int *)&Setting::Rendering::representation,
									   "Ball and stick\0Van der Waals\0Bond\0" ) )
					{
						for ( Model::ModelMolecule * const molecule : VTXApp::get().getScene().getMolecules() )
						{
							molecule->setRepresentation();
						}
					}

					if ( ImGui::Combo( LOCALE( "MainMenu.Settings.ColorMode" ),
									   (int *)&Setting::Rendering::colorMode,
									   "Atom\0Residue\0Chain\0" ) )
					{
						for ( Model::ModelMolecule * const molecule : VTXApp::get().getScene().getMolecules() )
						{
							molecule->setColorMode();
						}
					}

					if ( ImGui::Combo( LOCALE( "MainMenu.Settings.Shading" ),
									   (int *)&Setting::Rendering::shading,
									   "Lambert\0Blinn Phong\0Toon\0" ) )
					{ VTXApp::get().setRendererShading(); }

					// ImGui::Checkbox( LOCALE( "MainMenu.Settings.AA" ), &Setting::Rendering::useAA );
					ImGui::Checkbox( LOCALE( "MainMenu.Settings.SSAO" ), &Setting::Rendering::useSSAO );

					ImGui::Separator();

					ImGui::Checkbox( LOCALE( "MainMenu.Settings.InverseYAxis" ), &Setting::Controller::yAxisInverted );

					ImGui::EndMenu();
				}

				ImGuiIO & io = ImGui::GetIO();
				ImGui::Text( "FPS: %.0f", io.Framerate );

				ImGui::PopStyleVar();
				ImGui::EndMainMenuBar();
			}
		}
	} // namespace UI
} // namespace VTX
