#include "menu.hpp"
#include "action/main.hpp"
#include "action/path.hpp"
#include "action/setting.hpp"
#include "define.hpp"
#include "imgui/imgui_internal.h"
#include "style.hpp"
#include "util/filesystem.hpp"
#include "util/logger.hpp"
#include "util/time.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace UI
	{
		bool Menu::_drawHeader()
		{
			ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding,
								 ImVec2( Style::IMGUI_STYLE_MENUBAR_PADDING, Style::IMGUI_STYLE_MENUBAR_PADDING ) );
			return ImGui::BeginMainMenuBar();
		}

		void Menu::_drawFooter()
		{
			ImGui::PopStyleVar();
			ImGui::EndMainMenuBar();

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
				if ( paths.size() > 0 )
				{
					VTX_ACTION( new Action::Main::Open( paths ) );
				}
				_openFileDialog = nullptr;
			}

			// Open file dialog.
			if ( _saveFileDialog && _saveFileDialog->ready() )
			{
				std::string file = _saveFileDialog->result();
				VTX_ACTION( new Action::Main::Save( new Path( file ) ) );
				_saveFileDialog = nullptr;
			}
		}

		void Menu::_drawContent()
		{
			// Main menu.
			if ( ImGui::BeginMenu( LOCALE( "MainMenu.Menu" ), isVisiblePtr() ) )
			{
				// New.
				if ( ImGui::MenuItem( LOCALE( "MainMenu.Menu.New" ) /*, "Ctrl+N"*/ ) )
				{
					VTX_ACTION( new Action::Main::New() );
				}

				// Open.
				if ( ImGui::MenuItem( LOCALE( "MainMenu.Menu.Open" ) ) )
				{
					// TODO: file filters.
					_openFileDialog = std::shared_ptr<pfd::open_file>( new pfd::open_file(
						LOCALE( "MainMenu.Menu.Open.ChooseFile" ), "", { "All Files", "*" }, pfd::opt::multiselect ) );
				}

				if ( ImGui::MenuItem( LOCALE( "Save" ) ) )
				{
					_saveFileDialog = std::shared_ptr<pfd::save_file>(
						new pfd::save_file( LOCALE( "MainMenu.Menu.Open.ChooseFile" ),
											"",
											{ "VTX file (.vtx)", "*.vtx" },
											pfd::opt::none ) );
				}

				ImGui::Separator();

				// Quit.
				if ( ImGui::MenuItem( LOCALE( "MainMenu.Menu.Quit" ) ) )
				{
					VTX_ACTION( new Action::Main::Quit() );
				}

				ImGui::EndMenu();
			}

			// Display.
			if ( ImGui::BeginMenu( LOCALE( "MainMenu.Display" ), isVisiblePtr() ) )
			{
				bool showRender = VTXApp::get().getUI().getComponentByName( ID::UI::RENDER )->isVisible();
				if ( ImGui::Checkbox( LOCALE( "MainMenu.Display.Render" ), &showRender ) )
				{
					VTX_ACTION( new Action::Main::ActiveUIComponent( ID::UI::RENDER, showRender ) );
				}
				ImGui::Separator();
				ImGui::Separator();
				bool showScene = VTXApp::get().getUI().getComponentByName( ID::UI::SCENE )->isVisible();
				if ( ImGui::Checkbox( LOCALE( "MainMenu.Display.Scene" ), &showScene ) )
				{
					VTX_ACTION( new Action::Main::ActiveUIComponent( ID::UI::SCENE, showScene ) );
				}
				bool showInspector = VTXApp::get().getUI().getComponentByName( ID::UI::INSPECTOR )->isVisible();
				if ( ImGui::Checkbox( LOCALE( "MainMenu.Display.Inspector" ), &showInspector ) )
				{
					VTX_ACTION( new Action::Main::ActiveUIComponent( ID::UI::INSPECTOR, showInspector ) );
				}
				bool showConsole = VTXApp::get().getUI().getComponentByName( ID::UI::CONSOLE )->isVisible();
				if ( ImGui::Checkbox( LOCALE( "MainMenu.Display.Console" ), &showConsole ) )
				{
					VTX_ACTION( new Action::Main::ActiveUIComponent( ID::UI::CONSOLE, showConsole ) );
				}
				bool showCameraEditor = VTXApp::get().getUI().getComponentByName( ID::UI::CAMERA_EDITOR )->isVisible();
				if ( ImGui::Checkbox( LOCALE( "MainMenu.Display.CameraEditor" ), &showCameraEditor ) )
				{
					VTX_ACTION( new Action::Main::ActiveUIComponent( ID::UI::CAMERA_EDITOR, showCameraEditor ) );
				}

				ImGui::Separator();
				if ( ImGui::MenuItem( LOCALE( "MainMenu.Display.CloseAll" ) ) )
				{
					VTX_ACTION( new Action::Main::ActiveUIComponent( ID::UI::SCENE, false ) );
					VTX_ACTION( new Action::Main::ActiveUIComponent( ID::UI::INSPECTOR, false ) );
					VTX_ACTION( new Action::Main::ActiveUIComponent( ID::UI::CONSOLE, false ) );
					VTX_ACTION( new Action::Main::ActiveUIComponent( ID::UI::CAMERA_EDITOR, false ) );
				}
				if ( ImGui::MenuItem( LOCALE( "MainMenu.Display.ShowAll" ) ) )
				{
					VTX_ACTION( new Action::Main::ActiveUIComponent( ID::UI::SCENE, true ) );
					VTX_ACTION( new Action::Main::ActiveUIComponent( ID::UI::INSPECTOR, true ) );
					VTX_ACTION( new Action::Main::ActiveUIComponent( ID::UI::CONSOLE, true ) );
					VTX_ACTION( new Action::Main::ActiveUIComponent( ID::UI::CAMERA_EDITOR, true ) );
				}
				ImGui::EndMenu();
			}

			// Export
			if ( ImGui::BeginMenu( LOCALE( "MainMenu.Export" ), isVisiblePtr() ) )
			{
				if ( ImGui::MenuItem( LOCALE( "MainMenu.Export.Snapshot" ) ) )
				{
					VTX_ACTION( new Action::Main::Snapshot(
						Worker::Snapshoter::MODE::GL,
						Util::Filesystem::getSnapshotsPath( Util::Time::getTimestamp() + ".png" ) ) );
				}
				if ( ImGui::MenuItem( LOCALE( "MainMenu.Export.RenderCPU" ) ) )
				{
					VTX_ACTION( new Action::Main::Snapshot(
						Worker::Snapshoter::MODE::RT_CPU,
						Util::Filesystem::getRendersPath( Util::Time::getTimestamp() + ".png" ) ) );
				}
				if ( ImGui::MenuItem( LOCALE( "MainMenu.Export.RenderOptix" ) ) )
				{
					VTX_ACTION( new Action::Main::Snapshot(
						Worker::Snapshoter::MODE::RT_OPTIX,
						Util::Filesystem::getRendersPath( Util::Time::getTimestamp() + ".png" ) ) );
				}
				if ( ImGui::MenuItem( LOCALE( "MainMenu.Export.Video" ) ) )
				{
					VTX_ACTION( new Action::Path::ExportVideo( VTXApp::get().getScene().getPaths()[ 0 ],
															   Worker::Snapshoter::MODE::GL ) );
				}
				if ( ImGui::MenuItem( LOCALE( "MainMenu.Export.VideoRT" ) ) )
				{
					VTX_ACTION( new Action::Path::ExportVideo( VTXApp::get().getScene().getPaths()[ 0 ],
															   Worker::Snapshoter::MODE::RT_CPU ) );
				}
				ImGui::EndMenu();
			}

			// Settings.
			if ( ImGui::BeginMenu( LOCALE( "MainMenu.Settings" ), isVisiblePtr() ) )
			{
				// Theme.
				const char * themes[]
					= { LOCALE( "Enum.Theme.Light" ), LOCALE( "Enum.Theme.Dark" ), LOCALE( "Enum.Theme.Classic" ) };
				int theme = (int)VTX_SETTING().theme;
				if ( ImGui::Combo( LOCALE( "MainMenu.Settings.Theme" ), &theme, themes, 3 ) )
				{
					VTX_ACTION( new Action::Setting::ChangeTheme( (Style::THEME)theme ) );
				}

				ImGui::Separator();

				// Symbol display.
				const char * values[] = { LOCALE( "Enum.SymbolDisplay.Short" ), LOCALE( "Enum.SymbolDisplay.Long" ) };
				int			 symbolDisplayMode = (int)VTX_SETTING().symbolDisplayMode;
				if ( ImGui::Combo( LOCALE( "MainMenu.Settings.SymbolDisplay" ), &symbolDisplayMode, values, 2 ) )
				{
					VTX_ACTION(
						new Action::Setting::ChangeDisplayMode( (Style::SYMBOL_DISPLAY_MODE)symbolDisplayMode ) );
				}

				ImGui::Separator();

				// Active renderer.
				bool isActive = VTX_SETTING().activeRenderer;
				if ( ImGui::Checkbox( LOCALE( "MainMenu.Settings.Rendering" ), &isActive ) )
				{
					VTX_ACTION( new Action::Setting::ActiveRenderer( isActive ) );
				}

				// Background color.
				Color::Rgb bgColor = VTX_SETTING().backgroundColor;
				if ( ImGui::ColorEdit3( LOCALE( "MainMenu.Settings.BackgroundColor" ), bgColor.getRGB() ),
					 ImGuiColorEditFlags_NoAlpha )
				{
					VTX_ACTION( new Action::Setting::ChangeBackgroundColor( bgColor ) );
				}
				ImGui::Separator();

				// Light color.
				Color::Rgb lightColor = VTX_SETTING().lightColor;
				if ( ImGui::ColorEdit3( LOCALE( "MainMenu.Settings.LightColor" ), lightColor.getRGB() ),
					 ImGuiColorEditFlags_NoAlpha )
				{
					VTX_ACTION( new Action::Setting::ChangeLightColor( lightColor ) );
				}

				// Representation.
				const char * representations[] = { LOCALE( "Enum.Representation.BallsAndSticks" ),
												   LOCALE( "Enum.Representation.VanDerWaals" ),
												   LOCALE( "Enum.Representation.Sticks" ),
												   LOCALE( "Enum.Representation.SAS" ),
												   LOCALE( "Enum.Representation.Cartoon" ) };
				int			 representation	   = (int)VTX_SETTING().representation;
				if ( ImGui::Combo( LOCALE( "MainMenu.Settings.Representation" ), &representation, representations, 5 ) )
				{
					VTX_ACTION(
						new Action::Setting::ChangeRepresentation( ( Generic::REPRESENTATION )( representation ) ) );
				}

				// Allow to control bonds radius if the representation show them.
				if ( representation == int( Generic::REPRESENTATION::BALL_AND_STICK ) )
				{
					float atomsRadius = VTX_SETTING().atomsRadius;
					if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.AtomsRadius" ),
											 &atomsRadius,
											 Setting::ATOMS_RADIUS_MIN,
											 Setting::ATOMS_RADIUS_MAX,
											 "%.2f \u00c5" ) )
					{
						VTX_ACTION( new Action::Setting::ChangeAtomsRadius( atomsRadius ) );
					}
				}
				if ( representation == int( Generic::REPRESENTATION::STICK )
					 || representation == int( Generic::REPRESENTATION::BALL_AND_STICK ) )
				{
					float bondsRadius = VTX_SETTING().bondsRadius;
					if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.BondsRadius" ),
											 &bondsRadius,
											 Setting::BONDS_RADIUS_MIN,
											 Setting::BONDS_RADIUS_MAX,
											 "%.2f \u00c5" ) )
					{
						VTX_ACTION( new Action::Setting::ChangeBondsRadius( bondsRadius ) );
					}
				}

				// Color mode.
				const char * modes[]   = { LOCALE( "Enum.ColorMode.Atom" ),
										   LOCALE( "Enum.ColorMode.Residue" ),
										   LOCALE( "Enum.ColorMode.Chain" ),
										   LOCALE( "Enum.ColorMode.Protein" ) };
				int			 colorMode = (int)VTX_SETTING().colorMode;
				if ( ImGui::Combo( LOCALE( "MainMenu.Settings.ColorMode" ), &colorMode, modes, 4 ) )
				{
					VTX_ACTION( new Action::Setting::ChangeColorMode( (Generic::COLOR_MODE)colorMode ) );
				}

				// Shading.
				const char * shadings[] = { LOCALE( "Enum.Shading.Diffuse" ),
											LOCALE( "Enum.Shading.Glossy" ),
											LOCALE( "Enum.Shading.Toon" ),
											LOCALE( "Enum.Shading.FlatColor" ) };
				int			 shading	= (int)VTX_SETTING().shading;
				if ( ImGui::Combo( LOCALE( "MainMenu.Settings.Shading" ), &shading, shadings, 4 ) )
				{
					VTX_ACTION( new Action::Setting::ChangeShading( (Renderer::SHADING)shading ) );
				}

				// Camera.
				float camNear = VTX_SETTING().cameraNear;
				if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.CamNear" ),
										 &camNear,
										 Setting::CAMERA_NEAR_MIN,
										 Setting::CAMERA_NEAR_MAX,
										 "%.0f" ) )
				{
					VTX_ACTION( new Action::Setting::ChangeCameraClip( camNear, VTX_SETTING().cameraFar ) );
				}
				float camFar = VTX_SETTING().cameraFar;
				if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.CamFar" ),
										 &camFar,
										 Setting::CAMERA_FAR_MIN,
										 Setting::CAMERA_FAR_MAX,
										 "%.0f" ) )
				{
					VTX_ACTION( new Action::Setting::ChangeCameraClip( VTX_SETTING().cameraNear, camFar ) );
				}
				float camFov = VTX_SETTING().cameraFov;
				if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.CamFov" ),
										 &camFov,
										 Setting::CAMERA_FOV_MIN,
										 Setting::CAMERA_FOV_MAX ) )
				{
					VTX_ACTION( new Action::Setting::ChangeCameraFov( camFov ) );
				}
				bool camPerspective = VTX_SETTING().cameraPerspective;
				if ( ImGui::Checkbox( LOCALE( "MainMenu.Settings.CamPerspective" ), &camPerspective ) )
				{
					VTX_ACTION( new Action::Setting::ChangeCameraProjection( camPerspective ) );
				}

				// VSYNC.
				bool useVSync = VTX_SETTING().activeVSync;
				if ( ImGui::Checkbox( LOCALE( "MainMenu.Settings.VSync" ), &useVSync ) )
				{
					VTX_ACTION( new Action::Setting::ActiveVerticalSync( useVSync ) );
				}
				ImGui::Separator();

				// SSAO.
				bool useSSAO = VTX_SETTING().activeAO;
				if ( ImGui::Checkbox( LOCALE( "MainMenu.Settings.SSAO" ), &useSSAO ) )
				{
					VTX_ACTION( new Action::Setting::ActiveAO( useSSAO ) );
				}
				if ( VTX_SETTING().activeAO )
				{
					int aoIntensity = VTX_SETTING().aoIntensity;
					if ( ImGui::SliderInt( LOCALE( "MainMenu.Settings.AOIntensity" ),
										   &aoIntensity,
										   Setting::AO_INTENSITY_MIN,
										   Setting::AO_INTENSITY_MAX ) )
					{
						VTX_ACTION( new Action::Setting::ChangeAOIntensity( aoIntensity ) );
					}

					int aoBlurSize = VTX_SETTING().aoBlurSize;
					if ( ImGui::SliderInt( LOCALE( "MainMenu.Settings.AOBlurSize" ),
										   &aoBlurSize,
										   Setting::AO_BLUR_SIZE_MIN,
										   Setting::AO_BLUR_SIZE_MAX ) )
					{
						VTX_ACTION( new Action::Setting::ChangeAOBlurSize( aoBlurSize ) );
					}
				}
				ImGui::Separator();

				// Outline.
				bool useOutline = VTX_SETTING().activeOutline;
				if ( ImGui::Checkbox( LOCALE( "MainMenu.Settings.Outline" ), &useOutline ) )
				{
					VTX_ACTION( new Action::Setting::ActiveOutline( useOutline ) );
				}
				if ( VTX_SETTING().activeOutline )
				{
					Color::Rgb outlineColor = VTX_SETTING().outlineColor;
					if ( ImGui::ColorEdit3( LOCALE( "MainMenu.Settings.OutlineColor" ), outlineColor.getRGB() ) )
					{
						VTX_ACTION( new Action::Setting::ChangeOutlineColor( outlineColor ) );
					}
				}
				ImGui::Separator();

				// Fog.
				bool useFog = VTX_SETTING().activeFog;
				if ( ImGui::Checkbox( LOCALE( "MainMenu.Settings.Fog" ), &useFog ) )
				{
					VTX_ACTION( new Action::Setting::ActiveFog( useFog ) );
				}
				if ( VTX_SETTING().activeFog )
				{
					float fogNear = VTX_SETTING().fogNear;
					if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.FogNear" ),
											 &fogNear,
											 Setting::FOG_NEAR_MIN,
											 Setting::FOG_NEAR_MAX,
											 "%.1f" ) )
					{
						VTX_ACTION( new Action::Setting::ChangeFogNear( fogNear ) );
					}
					float fogFar = VTX_SETTING().fogFar;
					if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.FogFar" ),
											 &fogFar,
											 Setting::FOG_FAR_MIN,
											 Setting::FOG_FAR_MAX,
											 "%.1f" ) )
					{
						VTX_ACTION( new Action::Setting::ChangeFogFar( fogFar ) );
					}
					float fogDensity = VTX_SETTING().fogDensity;
					if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.FogDensity" ), &fogDensity, 0.f, 1.f, "%.1f" ) )
					{
						VTX_ACTION( new Action::Setting::ChangeFogDensity( fogDensity ) );
					}
					Color::Rgb fogColor = VTX_SETTING().fogColor;
					if ( ImGui::ColorEdit3( LOCALE( "MainMenu.Settings.FogColor" ), fogColor.getRGB() ) )
					{
						VTX_ACTION( new Action::Setting::ChangeFogColor( fogColor ) );
					}
				}
				ImGui::Separator();

				// AA.
				bool useAA = VTX_SETTING().activeAA;
				if ( ImGui::Checkbox( LOCALE( "MainMenu.Settings.AA" ), &useAA ) )
				{
					VTX_ACTION( new Action::Setting::ActiveAA( useAA ) );
				}
				ImGui::Separator();

				// Auto rotate.
				Vec3f autoRotateSpeed = VTX_SETTING().autoRotationSpeed;
				if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.AutoRotateXSpeed" ),
										 &autoRotateSpeed.x,
										 Setting::AUTO_ROTATE_SPEED_MIN,
										 Setting::AUTO_ROTATE_SPEED_MAX ) )
				{
					VTX_ACTION( new Action::Setting::ChangeAutoRotateSpeed( autoRotateSpeed ) );
				}
				if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.AutoRotateYSpeed" ),
										 &autoRotateSpeed.y,
										 Setting::AUTO_ROTATE_SPEED_MIN,
										 Setting::AUTO_ROTATE_SPEED_MAX ) )
				{
					VTX_ACTION( new Action::Setting::ChangeAutoRotateSpeed( autoRotateSpeed ) );
				}
				if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.AutoRotateZSpeed" ),
										 &autoRotateSpeed.z,
										 Setting::AUTO_ROTATE_SPEED_MIN,
										 Setting::AUTO_ROTATE_SPEED_MAX ) )
				{
					VTX_ACTION( new Action::Setting::ChangeAutoRotateSpeed( autoRotateSpeed ) );
				}

				ImGui::Separator();

				// Controller speed.
				float translationSpeed = VTX_SETTING().translationSpeed;
				if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.TranslationSpeed" ),
										 &translationSpeed,
										 Setting::CONTROLLER_TRANSLATION_SPEED_MIN,
										 Setting::CONTROLLER_TRANSLATION_SPEED_MAX ) )
				{
					VTX_ACTION( new Action::Setting::ChangeTranslationSpeed( translationSpeed ) );
				}
				float translationFactorSpeed = VTX_SETTING().translationFactorSpeed;
				if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.TranslationFactorSpeed" ),
										 &translationFactorSpeed,
										 Setting::CONTROLLER_TRANSLATION_FACTOR_MIN,
										 Setting::CONTROLLER_TRANSLATION_FACTOR_MAX ) )
				{
					VTX_ACTION( new Action::Setting::ChangeTranslationFactorSpeed( translationFactorSpeed ) );
				}
				float rotationSpeed = VTX_SETTING().rotationSpeed;
				if ( ImGui::SliderFloat( LOCALE( "MainMenu.Settings.RotationSpeed" ),
										 &rotationSpeed,
										 Setting::CONTROLLER_ROTATION_SPEED_MIN,
										 Setting::CONTROLLER_ROTATION_SPEED_MAX ) )
				{
					VTX_ACTION( new Action::Setting::ChangeRotationSpeed( rotationSpeed ) );
				}

				// Invert y axis.
				bool yAxisInverted = VTX_SETTING().yAxisInverted;
				if ( ImGui::Checkbox( LOCALE( "MainMenu.Settings.InverseYAxis" ), &yAxisInverted ) )
				{
					VTX_ACTION( new Action::Setting::ActiveYAxisInversion( yAxisInverted ) );
				}

				ImGui::Separator();

				// Load.
				if ( ImGui::MenuItem( LOCALE( "Load" ) ) )
				{
					VTX_ACTION( new Action::Setting::Load() );
				}

				// Save.
				if ( ImGui::MenuItem( LOCALE( "Save" ) ) )
				{
					VTX_ACTION( new Action::Setting::Save() );
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
					VTX_ACTION( new Action::Main::OpenApi( id ) );
				}
			}

			ImGui::PopItemWidth();

			// Undo/redo.
			/*
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
			*/

			// Merge.
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

			// Camera.
			if ( ImGui::Button( LOCALE( "MainMenu.Camera" ) ) )
			{
				VTX_ACTION( new Action::Main::ChangeCameraController() );
			}

			// Recenter Camera.
			if ( ImGui::Button( LOCALE( "MainMenu.RecenterCamera" ) ) )
			{
				VTX_ACTION( new Action::Main::RecenterCameraController() );
			}
		}
	} // namespace UI
} // namespace VTX
