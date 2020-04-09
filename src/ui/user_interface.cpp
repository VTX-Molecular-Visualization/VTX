#include "user_interface.hpp"
#include "camera_editor.hpp"
#include "console.hpp"
#include "define.hpp"
#include "exception.hpp"
#include "inspector.hpp"
#include "menu.hpp"
#include "modal/progress_bar.hpp"
#include "scene.hpp"
#include "setting.hpp"
#include "style.hpp"
#include "util/logger.hpp"
#include "util/opengl.hpp"
#include "vtx_app.hpp"
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
			switch ( Setting::UI::theme )
			{
			case Setting::UI::THEME::DARK: ImGui::StyleColorsDark(); break;
			case Setting::UI::THEME::CLASSIC: ImGui::StyleColorsClassic(); break;
			case Setting::UI::THEME::LIGHT:
			default: ImGui::StyleColorsLight(); break;
			}
		}

		void UserInterface::setVSync( const bool p_vsync ) { SDL_GL_SetSwapInterval( p_vsync ); }

		void UserInterface::_addItems()
		{
			// Windows.
			addItem( Generic::create<Menu>() );
			addItem( Generic::create<Console>() );
			addItem( Generic::create<Scene>() );
			addItem( Generic::create<Inspector>() );
			addItem( Generic::create<CameraEditor>() );

			// Modals.
			addItem( Generic::create<Modal::ProgressBar>() );
		}

		void UserInterface::_initSDL2()
		{
			VTX_INFO( "Initializing SDL2" );

			if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER ) != 0 )
			{ throw Exception::SDLException( SDL_GetError() ); }

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
				WINDOW_WIDTH,
				WINDOW_HEIGHT,
				SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI );

			if ( _window == nullptr ) { throw Exception::SDLException( SDL_GetError() ); }

			_glContext = SDL_GL_CreateContext( _window );
			if ( _glContext == nullptr ) { throw Exception::SDLException( SDL_GetError() ); }

			SDL_GL_MakeCurrent( _window, _glContext );
			setVSync(Setting::Rendering::useVSync);
		}

		void UserInterface::_initGL()
		{
			VTX_INFO( "Initializing OpenGL" );
			if ( gl3wInit() ) { throw Exception::GLException( "gl3wInit() failed" ); }

			if ( !gl3wIsSupported( OPENGL_VERSION_MAJOR, OPENGL_VERSION_MINOR ) )
			{ throw Exception::GLException( "OpenGL version not supported" ); }

#ifdef _DEBUG
			glEnable( GL_DEBUG_OUTPUT );
			// glEnable( GL_ARB_gpu_shader_fp64 );
			glDebugMessageCallback( VTX::Util::OpenGL::debugMessageCallback, NULL );
#endif
		}

		void UserInterface::_initIMGUI()
		{
			VTX_INFO( "Initializing IMGUI" );

			if ( !IMGUI_CHECKVERSION() ) { throw Exception::IMGUIException( "IMGUI_CHECKVERSION() failed" ); }

			ImGui::CreateContext();

			// Setup controls.
			ImGuiIO & io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

			// Configuration.
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			if ( IMGUI_ENABLE_VIEWPORTS ) { io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; }

			// Style.
			setTheme();
			ImGui::GetStyle().WindowRounding	= IMGUI_STYLE_ROUNDING;
			ImGui::GetStyle().ChildRounding		= IMGUI_STYLE_ROUNDING;
			ImGui::GetStyle().FrameRounding		= IMGUI_STYLE_ROUNDING;
			ImGui::GetStyle().GrabRounding		= IMGUI_STYLE_ROUNDING;
			ImGui::GetStyle().PopupRounding		= IMGUI_STYLE_ROUNDING;
			ImGui::GetStyle().ScrollbarRounding = IMGUI_STYLE_ROUNDING;
			ImGui::GetStyle().WindowBorderSize	= IMGUI_STYLE_WINDOW_BORDER;
			ImGui::GetStyle().WindowPadding		= ImVec2( IMGUI_STYLE_WINDOW_PADDING, IMGUI_STYLE_WINDOW_PADDING );

			// Setup Platform/Renderer bindings.
			if ( ImGui_ImplSDL2_InitForOpenGL( _window, _glContext ) == false )
			{ throw Exception::IMGUIException( "ImGui_ImplSDL2_InitForOpenGL failed" ); }
			if ( ImGui_ImplOpenGL3_Init( GLSL_VERSION.c_str() ) == false )
			{ throw Exception::IMGUIException( "ImGui_ImplOpenGL3_Init failed" ); }
		}

		void UserInterface::_disposeAll()
		{
			_disposeIMGUI();
			_disposeGL();
			_disposeSDL2();
		}

		void UserInterface::_disposeSDL2()
		{
			if ( _glContext ) { SDL_GL_DeleteContext( _glContext ); }
			if ( _window ) { SDL_DestroyWindow( _window ); }
			SDL_Quit();
		}

		void UserInterface::_disposeGL() {}

		void UserInterface::_disposeIMGUI()
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplSDL2_Shutdown();

			if ( ImGui::GetCurrentContext() != nullptr ) { ImGui::DestroyContext(); }
		}

		void UserInterface::_draw()
		{
			ImGuiIO & io = ImGui::GetIO();

			// New frame.
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame( _window );
			ImGui::NewFrame();

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
			ImGui::Begin( IMGUI_ID_MAIN_WINDOW, isVisiblePtr(), windowFlags );

			// Docking.
			ImGuiID			   dockSpaceId	  = ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE );
			ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
			ImGui::DockSpace( dockSpaceId, ImVec2( 0.0f, 0.0f ), dockSpaceFlags );

			// Viewport.
			SDL_GL_MakeCurrent( _window, _glContext );
			glViewport( 0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y );

			// Draw all components.
			_drawComponents();

			// Debug.
			if ( IMGUI_SHOW_DEMO ) { ImGui::ShowDemoWindow( NULL ); }

			// Main end.
			ImGui::End();

			// Render.
			ImGui::Render();

			if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}

			// GL.
			ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
			SDL_GL_SwapWindow( _window );
		}

		bool UserInterface::getEvent( SDL_Event & p_event ) const
		{
			bool hasEvent = SDL_PollEvent( &p_event );
			if ( hasEvent ) { ImGui_ImplSDL2_ProcessEvent( &p_event ); }
			return hasEvent;
		}

	} // namespace UI
} // namespace VTX
