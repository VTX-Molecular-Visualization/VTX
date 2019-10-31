#include "user_interface.hpp"
#include "../defines.hpp"
#include "../exceptions.hpp"
#include "../lib/imgui/imgui.h"
#include "../lib/imgui/imgui_impl_opengl3.h"
#include "../lib/imgui/imgui_impl_sdl.h"
#include "../util/logger.hpp"
#include "../vtx_app.hpp"
#include "component_menu.hpp"
#include <GL/gl3w.h>
#include <iostream>

namespace VTX
{
	namespace UI
	{
		UserInterface::UserInterface()
		{
			INF( "Creating user interface" );

			try
			{
				_initSDL2();
				_initGL();
				_initIMGUI();
			}

			catch ( const std::exception & p_e )
			{
				ERR( p_e.what() );
				_disposeAll();
				throw Exception::VTXException( "Can't start VTX" );
			}
		}

		UserInterface::~UserInterface() { _disposeAll(); }

		void UserInterface::_addComponents()
		{
			_addComponent( new ComponentMenu() );
		}

		void UserInterface::_initSDL2()
		{
			INF( "Initializing SDL2" );

			if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) != 0 )
			{ throw Exception::SDLException( SDL_GetError() ); }

			SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, 0 );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK,
								 SDL_GL_CONTEXT_PROFILE_CORE );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION,
								 OPENGL_VERSION_MAJOR );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION,
								 OPENGL_VERSION_MINOR );
			SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
			SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
			SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );

			// SDL_GetCurrentDisplayMode( 0, &m_displayMode );

			std::string title( "VTX" );
			_window = SDL_CreateWindow( title.c_str(),
										SDL_WINDOWPOS_CENTERED,
										SDL_WINDOWPOS_CENTERED,
										1200,
										1080,
										SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
											| SDL_WINDOW_RESIZABLE
											| SDL_WINDOW_ALLOW_HIGHDPI );

			if ( _window == nullptr )
			{ throw Exception::SDLException( SDL_GetError() ); }

			_glContext = SDL_GL_CreateContext( _window );
			if ( _glContext == nullptr )
			{ throw Exception::SDLException( SDL_GetError() ); }

			SDL_GL_MakeCurrent( _window, _glContext );
			SDL_GL_SetSwapInterval( true );
		}

		void UserInterface::_initGL()
		{
			INF( "Initializing OpenGL" );
			if ( gl3wInit() )
			{ throw Exception::GLException( "gl3wInit() failed" ); }

			if ( !gl3wIsSupported( OPENGL_VERSION_MAJOR,
								   OPENGL_VERSION_MINOR ) )
			{
				throw Exception::GLException( "OpenGL version not supported" );
			}

			const uchar * glVersion = glGetString( GL_VERSION );
			const uchar * glslVersion
				= glGetString( GL_SHADING_LANGUAGE_VERSION );
			const uchar * glVendor	 = glGetString( GL_VENDOR );
			const uchar * glRenderer = glGetString( GL_RENDERER );

			INF( "GL version: " + std::string( (const char *)glVersion ) );
			INF( "GLSL version: " + std::string( (const char *)glslVersion ) );
			INF( "GL device: " + std::string( (const char *)glVendor ) + " "
				 + std::string( (const char *)glRenderer ) );
		}

		void UserInterface::_initIMGUI()
		{
			INF( "Initializing IMGUI" );

			if ( !IMGUI_CHECKVERSION() )
			{
				throw Exception::IMGUIException(
					"IMGUI_CHECKVERSION() failed" );
			}

			ImGui::CreateContext();

			// Setup controls.
			ImGuiIO & io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

			// Style.
			ImGui::StyleColorsDark();

			// Setup Platform/Renderer bindings.
			if ( ImGui_ImplSDL2_InitForOpenGL( _window, _glContext ) == false )
			{
				throw Exception::IMGUIException(
					"ImGui_ImplSDL2_InitForOpenGL failed" );
			}
			if ( ImGui_ImplOpenGL3_Init() == false )
			{
				throw Exception::IMGUIException(
					"ImGui_ImplOpenGL3_Init failed" );
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
			if ( _glContext ) { SDL_GL_DeleteContext( _glContext ); }
			if ( _window ) { SDL_DestroyWindow( _window ); }
			SDL_Quit();
		}

		void UserInterface::_disposeGL() {}

		void UserInterface::_disposeIMGUI()
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplSDL2_Shutdown();

			if ( ImGui::GetCurrentContext() != nullptr )
			{ ImGui::DestroyContext(); }
		}

		void UserInterface::_draw()
		{
			// TODO: move ?
			// Quit event.
			SDL_Event event;
			while ( SDL_PollEvent( &event ) )
			{
				ImGui_ImplSDL2_ProcessEvent( &event );
				if ( ( event.type == SDL_QUIT )
					 || ( event.type == SDL_WINDOWEVENT
						  && event.window.event == SDL_WINDOWEVENT_CLOSE
						  && event.window.windowID
								 == SDL_GetWindowID( _window ) ) )
				{
					VTXApp::INSTANCE().stop();
					return;
				}
			}

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame( _window );
			ImGui::NewFrame();

			// ImGuiWindowFlags flags = 0;
			// flags |= ImGuiWindowFlags_MenuBar;

			_drawComponents();

			ImGui::Render();

			ImGuiIO & io = ImGui::GetIO();
			glViewport( 0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y );
			glClear( GL_COLOR_BUFFER_BIT );
			ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
			SDL_GL_SwapWindow( _window );
		}

	} // namespace UI
} // namespace VTX
