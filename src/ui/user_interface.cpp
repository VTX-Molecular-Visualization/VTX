#include "user_interface.hpp"
#include "../lib/imgui/imgui.h"
#include "../lib/imgui/imgui_impl_opengl3.h"
#include "../lib/imgui/imgui_impl_sdl.h"
#include <GL/gl3w.h>

namespace VTX
{
	namespace UI
	{
		UserInterface::UserInterface()
		{
			_initSDL2();
			_initIMGUI();
		}

		UserInterface::~UserInterface()
		{
			_disposeIMGUI();
			_disposeSDL2();
		}

		void UserInterface::_initSDL2()
		{
			if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) != 0 )
			{
				// throw SDLException( SDL_GetError() );
			}

			SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, 0 );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK,
								 SDL_GL_CONTEXT_PROFILE_CORE );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5 );

			SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
			SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );
			SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

			// SDL_GetCurrentDisplayMode( 0, &m_displayMode );

			_window = SDL_CreateWindow( "Test",
										SDL_WINDOWPOS_UNDEFINED,
										SDL_WINDOWPOS_UNDEFINED,
										1920,
										1080,
										SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
											| SDL_WINDOW_RESIZABLE
											| SDL_WINDOW_ALLOW_HIGHDPI );

			if ( _window == nullptr )
			{
				// throw SDLException( SDL_GetError() );
			}

			_glContext = SDL_GL_CreateContext( _window );
			if ( _glContext == nullptr )
			{
				// throw SDLException( SDL_GetError() );
			}

			SDL_GL_SetSwapInterval( true );
		}

		void UserInterface::_initIMGUI()
		{
			/*
			if ( !IMGUI_CHECKVERSION() )
				throw std::exception( "imgui check version error" );
				*/

			ImGui::CreateContext();
			ImGuiIO & io = ImGui::GetIO();
			// Enable Keyboard Controls
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

			// dark style
			ImGui::StyleColorsDark();

			// Setup Platform/Renderer bindings
			ImGui_ImplSDL2_InitForOpenGL( _window, _glContext );
			ImGui_ImplOpenGL3_Init();
		}

		void UserInterface::_disposeSDL2()
		{
			if ( _glContext ) { SDL_GL_DeleteContext( _glContext ); }
			if ( _window ) { SDL_DestroyWindow( _window ); }
			SDL_Quit();
		}

		void UserInterface::_disposeIMGUI()
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplSDL2_Shutdown();
			if ( ImGui::GetCurrentContext() != nullptr )
			{ ImGui::DestroyContext(); }
		}

		void UserInterface::display()
		{
			ImGuiIO & io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame( _window );
			ImGui::NewFrame();
			ImGui::Render();
			SDL_GL_MakeCurrent( _window, _glContext );
			glViewport( 0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y );

			ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
		}

	} // namespace UI
} // namespace VTX
