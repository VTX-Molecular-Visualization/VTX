#include "user_interface.hpp"
#include "define.hpp"
#include "exception.hpp"
#include "menu.hpp"
#include "modal/progress_bar.hpp"
#include "style.hpp"
#include "util/filesystem.hpp"
#include "util/logger.hpp"
#include "util/opengl.hpp"
#include "vtx_app.hpp"
#include "window/camera_editor.hpp"
#include "window/console.hpp"
#include "window/inspector.hpp"
#include "window/render.hpp"
#include "window/scene.hpp"
#include <imgui/ImGuizmo.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl.h>
#include <iostream>

namespace VTX
{
	namespace UI
	{
		UserInterface::UserInterface()
		{
			VTX_INFO( "Creating user interface" );

			// Menu.
			addItem( new Menu() );

			// Windows.
			addItem( new Window::Console() );
			addItem( new Window::Scene() );
			addItem( new Window::Inspector() );
			addItem( new Window::CameraEditor() );
			addItem( new Window::Render() );

			// Modals.
			addItem( new Modal::ProgressBar() );

			try
			{
				_initSDL2();
				_initGL();
				_initIMGUI();
			}

			catch ( const std::exception & p_e )
			{
				VTX_ERROR( p_e.what() );
				_disposeAll();
				throw Exception::VTXException( "Can't start VTX" );
			}
		}

		UserInterface::~UserInterface() { _disposeAll(); }

		void UserInterface::print() const
		{
			const uchar * glVersion	  = glGetString( GL_VERSION );
			const uchar * glslVersion = glGetString( GL_SHADING_LANGUAGE_VERSION );
			const uchar * glVendor	  = glGetString( GL_VENDOR );
			const uchar * glRenderer  = glGetString( GL_RENDERER );

			VTX_INFO( "GL version: " + std::string( (const char *)glVersion ) );
			VTX_INFO( "GLSL version: " + std::string( (const char *)glslVersion ) );
			VTX_INFO( "GL device: " + std::string( (const char *)glVendor ) + " "
					  + std::string( (const char *)glRenderer ) );
		}

		void UserInterface::setTheme() const
		{
			switch ( VTX_SETTING().theme )
			{
			case Style::THEME::DARK: ImGui::StyleColorsDark(); break;
			case Style::THEME::CLASSIC: ImGui::StyleColorsClassic(); break;
			case Style::THEME::LIGHT:
			default: ImGui::StyleColorsLight(); break;
			}
		}

		void UserInterface::setVSync( const bool p_vsync ) { SDL_GL_SetSwapInterval( p_vsync ); }

		void UserInterface::_initSDL2()
		{
			VTX_INFO( "Initializing SDL2" );

			if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER ) != 0 )
			{
				throw Exception::SDLException( SDL_GetError() );
			}

			SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, 0 );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, OPENGL_VERSION_MAJOR );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, OPENGL_VERSION_MINOR );
			SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
			SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
			SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );

			SDL_GetCurrentDisplayMode( 0, &_displayMode );

			std::string title( "VTX v" + std::to_string( VTX_VERSION_MAJOR ) + '.' + std::to_string( VTX_VERSION_MINOR )
							   + '.' + std::to_string( VTX_VERSION_REVISION ) );
#if _DEBUG
			title += " (DEBUG)";
#endif
			_window = SDL_CreateWindow(
				title.c_str(),
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED,
				VTX_SETTING().WINDOW_WIDTH_DEFAULT,
				VTX_SETTING().WINDOW_HEIGHT_DEFAULT,
				SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI );

			if ( _window == nullptr )
			{
				throw Exception::SDLException( SDL_GetError() );
			}

			_glContext = SDL_GL_CreateContext( _window );
			if ( _glContext == nullptr )
			{
				throw Exception::SDLException( SDL_GetError() );
			}

			SDL_GL_MakeCurrent( _window, _glContext );
			setVSync( VTX_SETTING().activeVSync );
		}

		void UserInterface::_initGL()
		{
			VTX_INFO( "Initializing OpenGL" );
			if ( gl3wInit() )
			{
				throw Exception::GLException( "gl3wInit() failed" );
			}

			if ( !gl3wIsSupported( OPENGL_VERSION_MAJOR, OPENGL_VERSION_MINOR ) )
			{
				throw Exception::GLException( "OpenGL version not supported" );
			}

#ifdef _DEBUG
			glEnable( GL_DEBUG_OUTPUT );
			glDebugMessageCallback( VTX::Util::OpenGL::debugMessageCallback, NULL );
#endif
		}

		void UserInterface::_initIMGUI()
		{
			VTX_INFO( "Initializing IMGUI" );

			if ( !IMGUI_CHECKVERSION() )
			{
				throw Exception::IMGUIException( "IMGUI_CHECKVERSION() failed" );
			}

			ImGui::CreateContext();

			// Setup controls.
			ImGuiIO & io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

			// Configuration.
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			if ( Style::IMGUI_ENABLE_VIEWPORTS )
			{
				io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
			}

			// Style.
			setTheme();
			ImGui::GetStyle().WindowRounding	= Style::IMGUI_STYLE_ROUNDING;
			ImGui::GetStyle().ChildRounding		= Style::IMGUI_STYLE_ROUNDING;
			ImGui::GetStyle().FrameRounding		= Style::IMGUI_STYLE_ROUNDING;
			ImGui::GetStyle().GrabRounding		= Style::IMGUI_STYLE_ROUNDING;
			ImGui::GetStyle().PopupRounding		= Style::IMGUI_STYLE_ROUNDING;
			ImGui::GetStyle().ScrollbarRounding = Style::IMGUI_STYLE_ROUNDING;
			ImGui::GetStyle().WindowBorderSize	= Style::IMGUI_STYLE_WINDOW_BORDER;
			ImGui::GetStyle().WindowPadding
				= ImVec2( Style::IMGUI_STYLE_WINDOW_PADDING, Style::IMGUI_STYLE_WINDOW_PADDING );

			// .ini location.
			io.IniFilename = Util::Filesystem::IMGUI_INI_FILE.c_str();

			// Setup Platform/Renderer bindings.
			if ( ImGui_ImplSDL2_InitForOpenGL( _window, _glContext ) == false )
			{
				throw Exception::IMGUIException( "ImGui_ImplSDL2_InitForOpenGL failed" );
			}
			if ( ImGui_ImplOpenGL3_Init( GLSL_VERSION.c_str() ) == false )
			{
				throw Exception::IMGUIException( "ImGui_ImplOpenGL3_Init failed" );
			}
		}

		void UserInterface::_disposeAll()
		{
			_disposeIMGUI();
			_disposeGL();
			_disposeSDL2();
		}

		void UserInterface::_disposeSDL2()
		{
			if ( _glContext )
			{
				SDL_GL_DeleteContext( _glContext );
			}
			if ( _window )
			{
				SDL_DestroyWindow( _window );
			}
			SDL_Quit();
		}

		void UserInterface::_disposeGL() {}

		void UserInterface::_disposeIMGUI()
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplSDL2_Shutdown();

			if ( ImGui::GetCurrentContext() != nullptr )
			{
				ImGui::DestroyContext();
			}
		}

		bool UserInterface::_drawHeader()
		{
			ImGuiIO & io = ImGui::GetIO();

			// New frame.
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame( _window );
			ImGui::NewFrame();
			ImGuizmo::BeginFrame();

			// Configuration.
			ImGuiViewport * viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos( viewport->Pos );
			ImGui::SetNextWindowSize( viewport->Size );
			ImGui::SetNextWindowViewport( viewport->ID );
			ImGui::SetNextWindowBgAlpha( 0.0f );
			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
										   | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
										   | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
										   | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

			// Main begin.
			return ImGui::Begin( Style::IMGUI_ID_MAIN_WINDOW, isVisiblePtr(), windowFlags );
		}

		void UserInterface::_drawFooter() { ImGui::End(); }

		void UserInterface::_drawContent()
		{
			ImGuiIO & io = ImGui::GetIO();

			// Viewport.
			SDL_GL_MakeCurrent( _window, _glContext );
			glViewport( 0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y );

			// Docking.
			ImGuiID			   dockSpaceId	  = ImGui::GetID( Style::IMGUI_ID_MAIN_DOCKSPACE );
			ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiWindowFlags_NoBackground;
			ImGui::DockSpace( dockSpaceId, ImVec2( 0.0f, 0.0f ), dockSpaceFlags );

			// Draw all components.
			_drawComponents();

			// Debug.
			if ( Style::IMGUI_SHOW_DEMO )
			{
				ImGui::ShowDemoWindow( NULL );
			}
		}

		void UserInterface::_draw()
		{
			if ( _drawHeader() == false )
			{
				_drawFooter();
				return;
			}

			_drawContent();
			_drawFooter();

			// Render.
			ImGui::Render();
			ImGuiIO & io = ImGui::GetIO();
			if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}

			// GL.
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
			SDL_GL_SwapWindow( _window );
		}

		bool UserInterface::getEvent( SDL_Event & p_event ) const
		{
			bool hasEvent = SDL_PollEvent( &p_event );
			if ( hasEvent )
			{
				ImGui_ImplSDL2_ProcessEvent( &p_event );
			}
			return hasEvent;
		}

	} // namespace UI
} // namespace VTX
