#include "component_menu.hpp"
#include "action/action_active_ssao.hpp"
#include "action/action_active_y_axis_inversion.hpp"
#include "action/action_change_ao_blur_size.hpp"
#include "action/action_change_ao_intensity.hpp"
#include "action/action_change_ao_radius.hpp"
#include "action/action_change_auto_rotate_speed.hpp"
#include "action/action_change_color_mode.hpp"
#include "action/action_change_display_mode.hpp"
#include "action/action_change_representation.hpp"
#include "action/action_change_rotation_speed.hpp"
#include "action/action_change_shading.hpp"
#include "action/action_change_theme.hpp"
#include "action/action_change_translation_factor_speed.hpp"
#include "action/action_change_translation_speed.hpp"
#include "action/action_export_video.hpp"
#include "action/action_new.hpp"
#include "action/action_open.hpp"
#include "action/action_quit.hpp"
#include "action/action_snapshot.hpp"
#include "define.hpp"
#include "imgui/imgui_internal.h"
#include "style.hpp"
#include "util/logger.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace UI
	{
		ComponentMenu::ComponentMenu( bool * const p_show,
									  bool * const p_showConsole,
									  bool * const p_showScene,
									  bool * const p_showInspector,
									  bool * const p_showCameraEditor ) :
			BaseComponent( p_show ),
			_showConsole( p_showConsole ), _showScene( p_showScene ), _showInspector( p_showInspector ),
			_showCameraEditor( p_showCameraEditor )
		{
		}

		void ComponentMenu::_draw()
		{
			ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding,
								 ImVec2( IMGUI_STYLE_MENUBAR_PADDING, IMGUI_STYLE_MENUBAR_PADDING ) );
			if ( ImGui::BeginMainMenuBar() )
			{
				// Main menu.
				if ( ImGui::BeginMenu( LOCALE( "MainMenu.Menu" ), _visible ) )
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
					{
#ifndef _DEBUG
						VTXApp::get().action( new Action::ActionQuit() );
						// ImGui::OpenPopup( "TEST" );
#else
						VTXApp::get().action( new Action::ActionQuit() );
#endif
					}

					ImGui::EndMenu();
				}

				// Display.
				if ( ImGui::BeginMenu( LOCALE( "MainMenu.Display" ), _visible ) )
				{
					ImGui::Checkbox( LOCALE( "MainMenu.Display.Scene" ), _showScene );
					ImGui::Checkbox( LOCALE( "MainMenu.Display.Inspector" ), _showInspector );
					ImGui::Checkbox( LOCALE( "MainMenu.Display.Console" ), _showConsole );
					ImGui::Checkbox( LOCALE( "MainMenu.Display.CameraEditor" ), _showCameraEditor );
					ImGui::Separator();
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Display.CloseAll" ) ) )
					{
						*_showScene		   = false;
						*_showInspector	   = false;
						*_showConsole	   = false;
						*_showCameraEditor = false;
					}
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Display.ShowAll" ) ) )
					{
						*_showScene		   = true;
						*_showInspector	   = true;
						*_showConsole	   = true;
						*_showCameraEditor = true;
					}
					ImGui::EndMenu();
				}

				// Export
				if ( ImGui::BeginMenu( LOCALE( "MainMenu.Export" ), _visible ) )
				{
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Export.Snapshot" ) ) )
					{ VTXApp::get().action( new Action::ActionSnapshot() ); }
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Export.Video" ) ) )
					{ VTXApp::get().action( new Action::ActionExportVideo() ); }
					ImGui::EndMenu();
				}

				// Settings.
				if ( ImGui::BeginMenu( LOCALE( "MainMenu.Settings" ), _visible ) )
				{
					// Theme.
					const char * themes[]
						= { LOCALE( "Enum.Theme.Light" ), LOCALE( "Enum.Theme.Dark" ), LOCALE( "Enum.Theme.Classic" ) };
					int theme = (int)Setting::UI::theme;
					if ( ImGui::Combo( LOCALE( "MainMenu.Settings.Theme" ), &theme, themes, 3 ) )
					{ VTXApp::get().action( new Action::ActionChangeTheme( (Setting::UI::THEME)theme ) ); }

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
					bool useSSAO = Setting::Rendering::useSSAO;
					if ( ImGui::Checkbox( LOCALE( "MainMenu.Settings.SSAO" ), &useSSAO ) )
					{ VTXApp::get().action( new Action::ActionActiveSSAO( useSSAO ) ); };

					float aoRadius = Setting::Rendering::aoRadius;
					if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.AORadius" ),
											 &aoRadius,
											 RENDERER_AO_RADIUS_MIN,
											 RENDERER_AO_RADIUS_MAX ) )
					{ VTXApp::get().action( new Action::ActionChangeAORadius( aoRadius ) ); }

					int aoIntensity = Setting::Rendering::aoIntensity;
					if ( ImGui::SliderInt( LOCALE( "MainMenu.Settings.AOIntensity" ),
										   &aoIntensity,
										   RENDERER_AO_INTENSITY_MIN,
										   RENDERER_AO_INTENSITY_MAX ) )
					{ VTXApp::get().action( new Action::ActionChangeAOIntensity( aoIntensity ) ); }
					ImGui::Separator();

					int aoBlurSize = Setting::Rendering::aoBlurSize;
					if ( ImGui::SliderInt( LOCALE( "MainMenu.Settings.AOBlurSize" ),
										   &aoBlurSize,
										   RENDERER_AO_BLUR_SIZE_MIN,
										   RENDERER_AO_BLUR_SIZE_MAX ) )
					{ VTXApp::get().action( new Action::ActionChangeAOBlurSize( aoBlurSize ) ); }
					ImGui::Separator();

					// Auto rotate.
					Vec3f autoRotateSpeed = Setting::Controller::autoRotateSpeed;
					if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.AutoRotateXSpeed" ),
											 &autoRotateSpeed.x,
											 AUTO_ROTATE_SPEED_MIN,
											 AUTO_ROTATE_SPEED_MAX ) )
					{ VTXApp::get().action( new Action::ActionChangeAutoRotateSpeed( autoRotateSpeed ) ); }
					if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.AutoRotateYSpeed" ),
											 &autoRotateSpeed.y,
											 AUTO_ROTATE_SPEED_MIN,
											 AUTO_ROTATE_SPEED_MAX ) )
					{ VTXApp::get().action( new Action::ActionChangeAutoRotateSpeed( autoRotateSpeed ) ); }
					if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.AutoRotateZSpeed" ),
											 &autoRotateSpeed.z,
											 AUTO_ROTATE_SPEED_MIN,
											 AUTO_ROTATE_SPEED_MAX ) )
					{ VTXApp::get().action( new Action::ActionChangeAutoRotateSpeed( autoRotateSpeed ) ); }

					ImGui::Separator();

					// Controller speed.
					float translationSpeed = Setting::Controller::translationSpeed;
					if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.TranslationSpeed" ),
											 &translationSpeed,
											 CONTROLLER_TRANSLATION_SPEED_MIN,
											 CONTROLLER_TRANSLATION_SPEED_MAX ) )
					{ VTXApp::get().action( new Action::ActionChangeTranslationSpeed( translationSpeed ) ); }
					float translationFactorSpeed = Setting::Controller::translationFactorSpeed;
					if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.TranslationFactorSpeed" ),
											 &translationFactorSpeed,
											 CONTROLLER_TRANSLATION_FACTOR_MIN,
											 CONTROLLER_TRANSLATION_FACTOR_MAX ) )
					{
						VTXApp::get().action(
							new Action::ActionChangeTranslationFactorSpeed( translationFactorSpeed ) );
					}
					float rotationSpeed = Setting::Controller::rotationSpeed;
					if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.RotationSpeed" ),
											 &rotationSpeed,
											 CONTROLLER_ROTATION_SPEED_MIN,
											 CONTROLLER_ROTATION_SPEED_MAX ) )
					{ VTXApp::get().action( new Action::ActionChangeRotationSpeed( rotationSpeed ) ); }

					// Invert y axis.
					bool yAxisInverted = Setting::Controller::yAxisInverted;
					if ( ImGui::Checkbox( LOCALE( "MainMenu.Settings.InverseYAxis" ), &yAxisInverted ) )
					{ VTXApp::get().action( new Action::ActionActiveYAxisInversion( yAxisInverted ) ); }

					ImGui::EndMenu();
				}

				// FPS.
				ImGuiIO & io = ImGui::GetIO();
				ImGui::Text( "FPS: %.0f", io.Framerate );

				// Undo/redo.
				bool popItem = false;
				if ( VTXApp::get().canUndo() == false )
				{
					ImGui::PushItemFlag( ImGuiItemFlags_Disabled, true );
					popItem = true;
				}
				if ( ImGui::Button( "Undo" ) ) { VTXApp::get().undo(); }
				if ( popItem ) { ImGui::PopItemFlag(); }

				popItem = false;
				if ( VTXApp::get().canRedo() == false )
				{
					ImGui::PushItemFlag( ImGuiItemFlags_Disabled, true );
					popItem = true;
				}
				if ( ImGui::Button( "Redo" ) ) { VTXApp::get().redo(); }
				if ( popItem ) { ImGui::PopItemFlag(); }

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
