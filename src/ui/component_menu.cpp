#include "component_menu.hpp"
#include "../action/action_change_color_mode.hpp"
#include "../action/action_change_display_mode.hpp"
#include "../action/action_change_representation.hpp"
#include "../action/action_change_shading.hpp"
#include "../action/action_change_theme.hpp"
#include "../action/action_new.hpp"
#include "../action/action_open.hpp"
#include "../action/action_quit.hpp"
#include "../action/action_snapshot.hpp"
#include "../defines.hpp"
#include "../style.hpp"
#include "../tool/snapshoter.hpp"
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
				// Main menu.
				if ( ImGui::BeginMenu( LOCALE( "MainMenu.Menu" ), _show ) )
				{
					// New.
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Menu.New" ) /*, "Ctrl+N"*/ ) )
					{ VTXApp::get().action( new Action::ActionNew() ); }

					// Open.
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Menu.Open" ) ) )
					{
						_openFileDialog
							= std::make_shared<pfd::open_file>( LOCALE( "MainMenu.Menu.Open.ChooseFile" ), "C:\\" );
					}

					// Quit.
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Menu.Quit" ) ) )
					{ VTXApp::get().action( new Action::ActionQuit() ); }

					ImGui::EndMenu();
				}

				// Display.
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

				// Export
				if ( ImGui::BeginMenu( LOCALE( "MainMenu.Export" ), _show ) )
				{
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Export.Snapshot" ) ) )
					{ VTXApp::get().action( new Action::ActionSnapshot() ); }
					ImGui::EndMenu();
				}

				// Settings.
				if ( ImGui::BeginMenu( LOCALE( "MainMenu.Settings" ), _show ) )
				{
					// Theme.
					const char * themes[]
						= { LOCALE( "Enum.Theme.Light" ), LOCALE( "Enum.Theme.Dark" ), LOCALE( "Enum.Theme.Classic" ) };
					if ( ImGui::Combo( LOCALE( "MainMenu.Settings.Theme" ), (int *)&Setting::UI::theme, themes, 3 ) )
					{ VTXApp::get().action( new Action::ActionChangeTheme() ); }

					ImGui::Separator();

					// Symbol display.
					const char * values[]
						= { LOCALE( "Enum.SymbolDisplay.Short" ), LOCALE( "Enum.SymbolDisplay.Long" ) };
					int symbolDisplayMode = (int)Setting::UI::symbolDisplayMode;
					if ( ImGui::Combo( LOCALE( "MainMenu.Settings.SymbolDisplay" ), &symbolDisplayMode, values, 2 ) )
					{
						VTXApp::get().action( new Action::ActionChangeDisplayMode(
							(Setting::UI::SYMBOL_DISPLAY_MODE)symbolDisplayMode ) );
					}

					ImGui::Separator();

					// Representation.
					const char * representations[] = { LOCALE( "Enum.Representation.BallsAndSticks" ),
													   LOCALE( "Enum.Representation.VanDerWaals" ),
													   LOCALE( "Enum.Representation.Sticks" ) };
					int			 representation	   = (int)Setting::Rendering::representation;
					if ( ImGui::Combo(
							 LOCALE( "MainMenu.Settings.Representation" ), &representation, representations, 3 ) )
					{
						VTXApp::get().action(
							new Action::ActionChangeRepresentation( (View::MOLECULE_REPRESENTATION)representation ) );
					}

					// Color mode.
					const char * modes[]   = { LOCALE( "Enum.ColorMode.Atom" ),
											   LOCALE( "Enum.ColorMode.Residue" ),
											   LOCALE( "Enum.ColorMode.Chain" ) };
					int			 colorMode = (int)Setting::Rendering::colorMode;
					if ( ImGui::Combo( LOCALE( "MainMenu.Settings.ColorMode" ), &colorMode, modes, 3 ) )
					{
						VTXApp::get().action(
							new Action::ActionChangeColorMode( (View::MOLECULE_COLOR_MODE)colorMode ) );
					}

					// Shading.
					const char * shadings[] = { LOCALE( "Enum.Shading.Lambert" ),
												LOCALE( "Enum.Shading.BlinnPhong" ),
												LOCALE( "Enum.Shading.Toon" ) };
					int			 shading	= (int)Setting::Rendering::shading;
					if ( ImGui::Combo( LOCALE( "MainMenu.Settings.Shading" ), &shading, shadings, 3 ) )
					{ VTXApp::get().action( new Action::ActionChangeShading( (Renderer::SHADING)shading ) ); }

					// SSAO.
					ImGui::Checkbox( LOCALE( "MainMenu.Settings.SSAO" ), &Setting::Rendering::useSSAO );

					ImGui::Separator();

					// Auto rotate.
					ImGui::SliderFloat( LOCALE( "MainMenu.Settings.AutoRotateXSpeed" ),
										&Setting::Controller::autoRotateSpeedX,
										AUTO_ROTATE_SPEED_MIN,
										AUTO_ROTATE_SPEED_MAX );
					ImGui::SliderFloat( LOCALE( "MainMenu.Settings.AutoRotateYSpeed" ),
										&Setting::Controller::autoRotateSpeedY,
										AUTO_ROTATE_SPEED_MIN,
										AUTO_ROTATE_SPEED_MAX );
					ImGui::SliderFloat( LOCALE( "MainMenu.Settings.AutoRotateZSpeed" ),
										&Setting::Controller::autoRotateSpeedZ,
										AUTO_ROTATE_SPEED_MIN,
										AUTO_ROTATE_SPEED_MAX );

					// Invert y axis.
					ImGui::Checkbox( LOCALE( "MainMenu.Settings.InverseYAxis" ), &Setting::Controller::yAxisInverted );

					ImGui::EndMenu();
				}

				// FPS.
				ImGuiIO & io = ImGui::GetIO();
				ImGui::Text( "FPS: %.0f", io.Framerate );

				// Undo/redo.
				if ( VTXApp::get().canUndo() )
				{
					if ( ImGui::Button( "Undo" ) ) { VTXApp::get().undo(); }
				}
				if ( VTXApp::get().canRedo() )
				{
					if ( ImGui::Button( "Redo" ) ) { VTXApp::get().redo(); }
				}

				ImGui::PopStyleVar();
				ImGui::EndMainMenuBar();
			}

			// Open file dialog.
			if ( _openFileDialog && _openFileDialog->ready() )
			{
				std::vector<std::string> result = _openFileDialog->result();
				if ( result.size() ) { VTXApp::get().action( new Action::ActionOpen( result[ 0 ] ) ); }
				_openFileDialog = nullptr;
			}
		}
	} // namespace UI
} // namespace VTX
