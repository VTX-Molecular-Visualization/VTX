#include "menu.hpp"
#include "action/active_renderer.hpp"
#include "action/active_ssao.hpp"
#include "action/active_ui_component.hpp"
#include "action/active_vertical_sync.hpp"
#include "action/active_y_axis_inversion.hpp"
#include "action/change_ao_blur_size.hpp"
#include "action/change_ao_factor.hpp"
#include "action/change_ao_intensity.hpp"
#include "action/change_ao_radius.hpp"
#include "action/change_auto_rotate_speed.hpp"
#include "action/change_color_mode.hpp"
#include "action/change_display_mode.hpp"
#include "action/change_representation.hpp"
#include "action/change_rotation_speed.hpp"
#include "action/change_shading.hpp"
#include "action/change_theme.hpp"
#include "action/change_translation_factor_speed.hpp"
#include "action/change_translation_speed.hpp"
#include "action/new.hpp"
#include "action/open.hpp"
#include "action/open_api.hpp"
#include "action/path_export.hpp"
#include "action/path_export_video.hpp"
#include "action/path_import.hpp"
#include "action/quit.hpp"
#include "action/snapshot.hpp"
#include "define.hpp"
#include "imgui/imgui_internal.h"
#include "style.hpp"
#include "util/filesystem.hpp"
#include "util/logger.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace UI
	{
		void Menu::_draw()
		{
			ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding,
								 ImVec2( IMGUI_STYLE_MENUBAR_PADDING, IMGUI_STYLE_MENUBAR_PADDING ) );

			if ( ImGui::BeginMainMenuBar() )
			{
				// Main menu.
				if ( ImGui::BeginMenu( LOCALE( "MainMenu.Menu" ), isVisiblePtr() ) )
				{
					// New.
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Menu.New" ) /*, "Ctrl+N"*/ ) )
					{
						VTX_ACTION( new Action::New() );
					}

					// Open.
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Menu.Open" ) ) )
					{
						_openFileDialog = std::shared_ptr<pfd::open_file>( new pfd::open_file(
							LOCALE( "MainMenu.Menu.Open.ChooseFile" ), "C://", { "All Files", "*" }, true ) );
					}

					// Quit.
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Menu.Quit" ) ) )
					{
						VTX_ACTION( new Action::Quit() );
					}

					ImGui::EndMenu();
				}

				// Display.
				if ( ImGui::BeginMenu( LOCALE( "MainMenu.Display" ), isVisiblePtr() ) )
				{
					bool showScene = VTXApp::get().getUI().getComponentByName( ID::UI::SCENE )->isVisible();
					if ( ImGui::Checkbox( LOCALE( "MainMenu.Display.Scene" ), &showScene ) )
					{
						VTX_ACTION( new Action::ActiveUIComponent( ID::UI::SCENE, showScene ) );
					}
					bool showInspector = VTXApp::get().getUI().getComponentByName( ID::UI::INSPECTOR )->isVisible();
					if ( ImGui::Checkbox( LOCALE( "MainMenu.Display.Inspector" ), &showInspector ) )
					{
						VTX_ACTION( new Action::ActiveUIComponent( ID::UI::INSPECTOR, showInspector ) );
					}
					bool showConsole = VTXApp::get().getUI().getComponentByName( ID::UI::CONSOLE )->isVisible();
					if ( ImGui::Checkbox( LOCALE( "MainMenu.Display.Console" ), &showConsole ) )
					{
						VTX_ACTION( new Action::ActiveUIComponent( ID::UI::CONSOLE, showConsole ) );
					}
					bool showCameraEditor
						= VTXApp::get().getUI().getComponentByName( ID::UI::CAMERA_EDITOR )->isVisible();
					if ( ImGui::Checkbox( LOCALE( "MainMenu.Display.CameraEditor" ), &showCameraEditor ) )
					{
						VTX_ACTION( new Action::ActiveUIComponent( ID::UI::CAMERA_EDITOR, showCameraEditor ) );
					}

					ImGui::Separator();
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Display.CloseAll" ) ) )
					{
						VTX_ACTION( new Action::ActiveUIComponent( ID::UI::SCENE, false ) );
						VTX_ACTION( new Action::ActiveUIComponent( ID::UI::INSPECTOR, false ) );
						VTX_ACTION( new Action::ActiveUIComponent( ID::UI::CONSOLE, false ) );
						VTX_ACTION( new Action::ActiveUIComponent( ID::UI::CAMERA_EDITOR, false ) );
					}
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Display.ShowAll" ) ) )
					{
						VTX_ACTION( new Action::ActiveUIComponent( ID::UI::SCENE, true ) );
						VTX_ACTION( new Action::ActiveUIComponent( ID::UI::INSPECTOR, true ) );
						VTX_ACTION( new Action::ActiveUIComponent( ID::UI::CONSOLE, true ) );
						VTX_ACTION( new Action::ActiveUIComponent( ID::UI::CAMERA_EDITOR, true ) );
					}
					ImGui::EndMenu();
				}

				// Import
				if ( ImGui::BeginMenu( LOCALE( "MainMenu.Import" ), isVisiblePtr() ) )
				{
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Import.Path" ) ) )
					{
						try
						{
							VTX_ACTION( new Action::PathImport( VTXApp::get().getScene().getPaths()[ 0 ] ) );
						}
						catch ( const std::exception & e )
						{
							VTX_WARNING( "Import failed: " + std::string( e.what() ) );
						}
					}
					ImGui::EndMenu();
				}

				// Export
				if ( ImGui::BeginMenu( LOCALE( "MainMenu.Export" ), isVisiblePtr() ) )
				{
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Export.Snapshot" ) ) )
					{
						VTX_ACTION( new Action::Snapshot(
							Worker::Snapshoter::MODE::GL,
							Util::Filesystem::getSnapshotsPath( Util::Time::getTimestamp() + ".png" ) ) );
					}
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Export.Render" ) ) )
					{
						VTX_ACTION( new Action::Snapshot(
							Worker::Snapshoter::MODE::RT,
							Util::Filesystem::getRendersPath( Util::Time::getTimestamp() + ".png" ) ) );
					}
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Export.Path" ) ) )
					{
						try
						{
							VTX_ACTION( new Action::PathExport( VTXApp::get().getScene().getPaths()[ 0 ] ) );
						}
						catch ( const std::exception & e )
						{
							VTX_WARNING( "Export failed: " + std::string( e.what() ) );
						}
					}
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Export.Video" ) ) )
					{
						VTX_ACTION( new Action::PathExportVideo( VTXApp::get().getScene().getPaths()[ 0 ],
																 Worker::Snapshoter::MODE::GL ) );
					}
					if ( ImGui::MenuItem( LOCALE( "MainMenu.Export.VideoRT" ) ) )
					{
						VTX_ACTION( new Action::PathExportVideo( VTXApp::get().getScene().getPaths()[ 0 ],
																 Worker::Snapshoter::MODE::RT ) );
					}
					ImGui::EndMenu();
				}

				// Settings.
				if ( ImGui::BeginMenu( LOCALE( "MainMenu.Settings" ), isVisiblePtr() ) )
				{
					// Theme.
					const char * themes[]
						= { LOCALE( "Enum.Theme.Light" ), LOCALE( "Enum.Theme.Dark" ), LOCALE( "Enum.Theme.Classic" ) };
					int theme = (int)Setting::UI::theme;
					if ( ImGui::Combo( LOCALE( "MainMenu.Settings.Theme" ), &theme, themes, 3 ) )
					{
						VTX_ACTION( new Action::ChangeTheme( (Setting::UI::THEME)theme ) );
					}

					ImGui::Separator();

					// Symbol display.
					const char * values[]
						= { LOCALE( "Enum.SymbolDisplay.Short" ), LOCALE( "Enum.SymbolDisplay.Long" ) };
					int symbolDisplayMode = (int)Setting::UI::symbolDisplayMode;
					if ( ImGui::Combo( LOCALE( "MainMenu.Settings.SymbolDisplay" ), &symbolDisplayMode, values, 2 ) )
					{
						VTX_ACTION(
							new Action::ChangeDisplayMode( (Setting::UI::SYMBOL_DISPLAY_MODE)symbolDisplayMode ) );
					}

					ImGui::Separator();

					// Active renderer.
					bool isActive = Setting::Rendering::isActive;
					if ( ImGui::Checkbox( LOCALE( "MainMenu.Settings.Rendering" ), &isActive ) )
					{
						VTX_ACTION( new Action::ActiveRenderer( isActive ) );
					};

					// Representation.
					const char * representations[] = { LOCALE( "Enum.Representation.BallsAndSticks" ),
													   LOCALE( "Enum.Representation.VanDerWaals" ),
													   LOCALE( "Enum.Representation.Sticks" ),
													   LOCALE( "Enum.Representation.SAS" ) };
					int			 representation	   = (int)Setting::Rendering::representation;
					if ( ImGui::Combo(
							 LOCALE( "MainMenu.Settings.Representation" ), &representation, representations, 4 ) )
					{
						VTX_ACTION( new Action::ChangeRepresentation( (View::MOLECULE_REPRESENTATION)representation ) );
					}

					// Color mode.
					const char * modes[]   = { LOCALE( "Enum.ColorMode.Atom" ),
											   LOCALE( "Enum.ColorMode.Residue" ),
											   LOCALE( "Enum.ColorMode.Chain" ),
											   LOCALE( "Enum.ColorMode.Protein" ) };
					int			 colorMode = (int)Setting::Rendering::colorMode;
					if ( ImGui::Combo( LOCALE( "MainMenu.Settings.ColorMode" ), &colorMode, modes, 4 ) )
					{
						VTX_ACTION( new Action::ChangeColorMode( (View::MOLECULE_COLOR_MODE)colorMode ) );
					}

					// Shading.
					const char * shadings[] = { LOCALE( "Enum.Shading.Lambert" ),
												LOCALE( "Enum.Shading.BlinnPhong" ),
												LOCALE( "Enum.Shading.Toon" ),
												LOCALE( "Enum.Shading.FlatColor" ) };
					int			 shading	= (int)Setting::Rendering::shading;
					if ( ImGui::Combo( LOCALE( "MainMenu.Settings.Shading" ), &shading, shadings, 4 ) )
					{
						VTX_ACTION( new Action::ChangeShading( (Renderer::SHADING)shading ) );
					}

					// VSYNC.
					bool useVSync = Setting::Rendering::useVSync;
					if ( ImGui::Checkbox( LOCALE( "MainMenu.Settings.VSync" ), &useVSync ) )
					{
						VTX_ACTION( new Action::ActiveVerticalSync( useVSync ) );
					};

					// SSAO.
					bool useSSAO = Setting::Rendering::useSSAO;
					if ( ImGui::Checkbox( LOCALE( "MainMenu.Settings.SSAO" ), &useSSAO ) )
					{
						VTX_ACTION( new Action::ActiveSSAO( useSSAO ) );
					};

					float aoRadius = Setting::Rendering::aoRadius;
					if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.AORadius" ),
											 &aoRadius,
											 RENDERER_AO_RADIUS_MIN,
											 RENDERER_AO_RADIUS_MAX ) )
					{
						VTX_ACTION( new Action::ChangeAORadius( aoRadius ) );
					}

					int aoIntensity = Setting::Rendering::aoIntensity;
					if ( ImGui::SliderInt( LOCALE( "MainMenu.Settings.AOIntensity" ),
										   &aoIntensity,
										   RENDERER_AO_INTENSITY_MIN,
										   RENDERER_AO_INTENSITY_MAX ) )
					{
						VTX_ACTION( new Action::ChangeAOIntensity( aoIntensity ) );
					}
					ImGui::Separator();

					int aoBlurSize = Setting::Rendering::aoBlurSize;
					if ( ImGui::SliderInt( LOCALE( "MainMenu.Settings.AOBlurSize" ),
										   &aoBlurSize,
										   RENDERER_AO_BLUR_SIZE_MIN,
										   RENDERER_AO_BLUR_SIZE_MAX ) )
					{
						VTX_ACTION( new Action::ChangeAOBlurSize( aoBlurSize ) );
					}
					ImGui::Separator();

					float aoFactor = Setting::Rendering::aoFactor;
					if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.AOFactor" ), &aoFactor, 0.f, 1.f ) )
					{
						VTX_ACTION( new Action::ChangeAOFactor( aoFactor ) );
					}
					ImGui::Separator();

					// Auto rotate.
					Vec3f autoRotateSpeed = Setting::Controller::autoRotateSpeed;
					if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.AutoRotateXSpeed" ),
											 &autoRotateSpeed.x,
											 AUTO_ROTATE_SPEED_MIN,
											 AUTO_ROTATE_SPEED_MAX ) )
					{
						VTX_ACTION( new Action::ChangeAutoRotateSpeed( autoRotateSpeed ) );
					}
					if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.AutoRotateYSpeed" ),
											 &autoRotateSpeed.y,
											 AUTO_ROTATE_SPEED_MIN,
											 AUTO_ROTATE_SPEED_MAX ) )
					{
						VTX_ACTION( new Action::ChangeAutoRotateSpeed( autoRotateSpeed ) );
					}
					if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.AutoRotateZSpeed" ),
											 &autoRotateSpeed.z,
											 AUTO_ROTATE_SPEED_MIN,
											 AUTO_ROTATE_SPEED_MAX ) )
					{
						VTX_ACTION( new Action::ChangeAutoRotateSpeed( autoRotateSpeed ) );
					}

					ImGui::Separator();

					// Controller speed.
					float translationSpeed = Setting::Controller::translationSpeed;
					if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.TranslationSpeed" ),
											 &translationSpeed,
											 CONTROLLER_TRANSLATION_SPEED_MIN,
											 CONTROLLER_TRANSLATION_SPEED_MAX ) )
					{
						VTX_ACTION( new Action::ChangeTranslationSpeed( translationSpeed ) );
					}
					float translationFactorSpeed = Setting::Controller::translationFactorSpeed;
					if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.TranslationFactorSpeed" ),
											 &translationFactorSpeed,
											 CONTROLLER_TRANSLATION_FACTOR_MIN,
											 CONTROLLER_TRANSLATION_FACTOR_MAX ) )
					{
						VTX_ACTION( new Action::ChangeTranslationFactorSpeed( translationFactorSpeed ) );
					}
					float rotationSpeed = Setting::Controller::rotationSpeed;
					if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.RotationSpeed" ),
											 &rotationSpeed,
											 CONTROLLER_ROTATION_SPEED_MIN,
											 CONTROLLER_ROTATION_SPEED_MAX ) )
					{
						VTX_ACTION( new Action::ChangeRotationSpeed( rotationSpeed ) );
					}

					// Invert y axis.
					bool yAxisInverted = Setting::Controller::yAxisInverted;
					if ( ImGui::Checkbox( LOCALE( "MainMenu.Settings.InverseYAxis" ), &yAxisInverted ) )
					{
						VTX_ACTION( new Action::ActiveYAxisInversion( yAxisInverted ) );
					}

					ImGui::EndMenu();
				}

				// FPS.
				const ImGuiIO & io = ImGui::GetIO();
				ImGui::Text( "FPS: %.0f", io.Framerate );

				// Open from API.
				static char name[ 5 ] = "";
				ImGui::PushItemWidth( 100 );
				if ( ImGui::InputText( LOCALE( "MainMenu.OpenAPI" ), name, IM_ARRAYSIZE( name ) ) )
				{
					std::string id = std::string( name );
					if ( id.size() == 4 )
					{
						VTX_ACTION( new Action::OpenApi( id ) );
					}
				}

				ImGui::PopItemWidth();

				// Undo/redo.
				bool popItem = false;
				if ( VTXApp::get().getActionManager().canUndo() == false )
				{
					ImGui::PushItemFlag( ImGuiItemFlags_Disabled, true );
					popItem = true;
				}
				if ( ImGui::Button( LOCALE( "MainMenu.Undo" ) ) )
				{
					VTXApp::get().getActionManager().undo();
				}
				if ( popItem )
				{
					ImGui::PopItemFlag();
				}

				popItem = false;
				if ( VTXApp::get().getActionManager().canRedo() == false )
				{
					ImGui::PushItemFlag( ImGuiItemFlags_Disabled, true );
					popItem = true;
				}
				if ( ImGui::Button( LOCALE( "MainMenu.Redo" ) ) )
				{
					VTXApp::get().getActionManager().redo();
				}
				if ( popItem )
				{
					ImGui::PopItemFlag();
				}

				if ( VTXApp::get().getScene().getMolecules().size() == 2 )
				{
					Model::Molecule * m1 = ( *( VTXApp::get().getScene().getMolecules().begin() ) ).first;
					Model::Molecule * m2 = ( *( ++VTXApp::get().getScene().getMolecules().begin() ) ).first;

					if ( m1->hasTopology() && m2->hasTopology() == false )
					{
						if ( ImGui::Button( "Merge" ) )
						{
							m2->mergeTopology( *m1 );
						}
					}
					else if ( m2->hasTopology() && m1->hasTopology() == false )
					{
						if ( ImGui::Button( "Merge" ) )
						{
							m1->mergeTopology( *m2 );
						}
					}
				}

				ImGui::PopStyleVar();
				ImGui::EndMainMenuBar();
			}

			// Open file dialog.
			if ( _openFileDialog && _openFileDialog->ready() )
			{
				std::vector<std::string> files = _openFileDialog->result();
				VTX_DEBUG( std::to_string( files.size() ) + " selected" );
				std::vector<Path *> paths = std::vector<Path *>();
				for ( std::string & file : files )
				{
					paths.emplace_back( new Path( file ) );
				}
				if ( paths.size() )
				{
					VTX_ACTION( new Action::Open( paths ) );
				}
				_openFileDialog = nullptr;
			}
		}
	} // namespace UI
} // namespace VTX
