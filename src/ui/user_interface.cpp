#include "user_interface.hpp"
#include "../defines.hpp"
#include "../exceptions.hpp"
#include "../lib/imgui/imgui.h"
#include "../lib/imgui/imgui_impl_opengl3.h"
#include "../lib/imgui/imgui_impl_sdl.h"
#include "../localization/language.hpp"
#include "../style.hpp"
#include "../util/logger.hpp"
#include "../vtx_app.hpp"
#include "component_console.hpp"
#include "component_menu.hpp"
#include <GL/gl3w.h>
#include <iostream>

namespace VTX
{
	namespace UI
	{
		UserInterface::UserInterface() : BaseComponent( NULL )
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

		void UserInterface::printInfos() const
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

		void UserInterface::_addComponents()
		{
			_addComponent( new ComponentMenu( &_showMenu, &_showConsole ) );
			_addComponent( new ComponentConsole( &_showConsole ) );
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

			// SDL_GetCurrentDisplayMode( 0, &m_displayMode );

			std::string title( "VTX v" + std::to_string( VTX_VERSION_MAJOR ) + '.'
							   + std::to_string( VTX_VERSION_MINOR ) );
#if _DEBUG
			title += " (DEV)";
#endif
			_window = SDL_CreateWindow(
				title.c_str(),
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED,
				WINDOWS_WIDTH,
				WINDOWS_HEIGHT,
				SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI );

			if ( _window == nullptr ) { throw Exception::SDLException( SDL_GetError() ); }

			_glContext = SDL_GL_CreateContext( _window );
			if ( _glContext == nullptr ) { throw Exception::SDLException( SDL_GetError() ); }

			SDL_GL_MakeCurrent( _window, _glContext );
			SDL_GL_SetSwapInterval( true );
		}

		void UserInterface::_initGL()
		{
			VTX_INFO( "Initializing OpenGL" );
			if ( gl3wInit() ) { throw Exception::GLException( "gl3wInit() failed" ); }

			if ( !gl3wIsSupported( OPENGL_VERSION_MAJOR, OPENGL_VERSION_MINOR ) )
			{ throw Exception::GLException( "OpenGL version not supported" ); }
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
			ImGui::StyleColorsLight();
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
			if ( ImGui_ImplOpenGL3_Init() == false )
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
			SDL_Event event;

			// TODO: move ?
			// Quit event.
			while ( SDL_PollEvent( &event ) )
			{
				ImGui_ImplSDL2_ProcessEvent( &event );
				if ( ( event.type == SDL_QUIT )
					 || ( event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE
						  && event.window.windowID == SDL_GetWindowID( _window ) ) )
				{
					VTXApp::get().stop();
					return;
				}
			}

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
			ImGui::Begin( IMGUI_ID_MAIN_WINDOW, _show, windowFlags );

			// Docking.
			ImGuiID			   dockSpaceId	  = ImGui::GetID( IMGUI_ID_MAIN_DOCKSPACE );
			ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
			ImGui::DockSpace( dockSpaceId, ImVec2( 0.0f, 0.0f ), dockSpaceFlags );

			// Draw all components.
			_drawComponents();
			if ( IMGUI_SHOW_DEMO ) { ImGui::ShowDemoWindow( NULL ); }

			// Main end.
			ImGui::End();

			// Render.
			ImGui::Render();
			glViewport( 0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y );
			glClear( GL_COLOR_BUFFER_BIT );
			ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

			if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
			{
				SDL_Window *  currentWindow	 = SDL_GL_GetCurrentWindow();
				SDL_GLContext currentContext = SDL_GL_GetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				SDL_GL_MakeCurrent( currentWindow, currentContext );
			}

			SDL_GL_SwapWindow( _window );
		}

	} // namespace UI
} // namespace VTX
