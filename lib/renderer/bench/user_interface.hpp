#ifndef __VTX_BENCH_USER_INTERFACE__
#define __VTX_BENCH_USER_INTERFACE__

#include "camera.hpp"
#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>
#include <renderer/gl/opengl_renderer.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>

namespace VTX::Bench
{

	class UserInterface
	{
	  public:
		UserInterface( const size_t p_width, const size_t p_height )
		{
			// Init SDL2.
			if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) != 0 )
			{
				throw std::runtime_error( "Failed to init SDL: " + std::string( SDL_GetError() ) );
			}

			SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, 0 );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5 );
			SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

			_window = SDL_CreateWindow(
				"VTX_RENDERER_BENCH",
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED,
				int( p_width ),
				int( p_height ),
				SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI );

			if ( _window == nullptr )
			{
				throw std::runtime_error( "Failed to create SDL2 window" );
			}

			_glContext = SDL_GL_CreateContext( _window );
			if ( _glContext == nullptr )
			{
				throw std::runtime_error( "Failed to coreate GL context: " + std::string( SDL_GetError() ) );
			}

			SDL_GL_MakeCurrent( _window, _glContext );
			SDL_GL_SetSwapInterval( _vsync );

			// Init ImGui.
			if ( !IMGUI_CHECKVERSION() )
			{
				throw std::runtime_error( "IMGUI_CHECKVERSION() failed" );
			}

			ImGui::CreateContext();
			ImGui::StyleColorsDark();

			if ( ImGui_ImplSDL2_InitForOpenGL( _window, _glContext ) == false )
			{
				throw std::runtime_error( "ImGui_ImplSDL2_InitForOpenGL failed" );
			}
			if ( ImGui_ImplOpenGL3_Init( "#version 450 core" ) == false )
			{
				throw std::runtime_error( "ImGui_ImplOpenGL3_Init failed" );
			}
		}

		~UserInterface()
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplSDL2_Shutdown();
			if ( ImGui::GetCurrentContext() != nullptr )
			{
				ImGui::DestroyContext();
			}
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

		inline double getTime() const { return SDL_GetTicks(); }
		inline float  getDeltaTime() const { return ImGui::GetIO().DeltaTime; }
		inline void * getProcAddress() { return reinterpret_cast<void *>( SDL_GL_GetProcAddress ); }
		inline void	  setVSync( const bool p_vsync )
		{
			_vsync = p_vsync;
			SDL_GL_SetSwapInterval( _vsync );
		}

		void draw( Renderer::GL::OpenGLRenderer * const p_renderer, Camera * const p_camera )
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();

			// Menu bar.
			if ( ImGui::BeginMainMenuBar() )
			{	  // Main menu.
				if ( ImGui::BeginMenu( "Menu" ) )
				{ // Quit.
					if ( ImGui::MenuItem( "Compile shaders" ) )
					{
						p_renderer->compileShaders();
					}

					ImGui::EndMenu();
				}

				ImGui::EndMainMenuBar();
			}

			// Camera.
			static float near = p_camera->getNear();
			static float far  = p_camera->getFar();
			static float fov  = p_camera->getFov();
			// static bool	 isPerspective = true;
			ImGui::Begin( "Camera" );
			// ImGui::Checkbox( "Perspective", &isPerspective );
			if ( ImGui::InputFloat( "Near", &near ) )
			{
				p_camera->setNear( near );
			}
			if ( ImGui::InputFloat( "Far", &far ) )
			{
				p_camera->setFar( far );
			}
			if ( ImGui::InputFloat( "Fov", &fov ) )
			{
				p_camera->setFov( fov );
			}
			ImGui::End();

			// Passes.
			static bool						  activeSSAO	   = p_renderer->isActiveSSAO();
			static bool						  activeOutline	   = p_renderer->isActiveOutline();
			static bool						  activeFXAA	   = p_renderer->isActiveFXAA();
			static Util::Color::Rgba		  colorBG		   = p_renderer->getColorBackground();
			static Util::Color::Rgba		  colorLight	   = p_renderer->getColorLight();
			static Util::Color::Rgba		  colorFog		   = p_renderer->getColorFog();
			static Util::Color::Rgba		  colorOutline	   = p_renderer->getColorOutline();
			static Util::Color::Rgba		  colorSelection   = p_renderer->getColorSelection();
			static float					  specularFactor   = p_renderer->getSpecularFactor();
			static float					  fogNear		   = p_renderer->getFogNear();
			static float					  fogFar		   = p_renderer->getFogFar();
			static float					  fogDensity	   = p_renderer->getFogDensity();
			static float					  ssaoIntensity	   = p_renderer->getSSAOIntensity();
			static float					  blurSize		   = p_renderer->getBlurSize();
			static float					  outlineSensivity = p_renderer->getOutlineSensivity();
			static float					  outlineThickness = p_renderer->getOutlineThickness();
			static Renderer::GL::ENUM_SHADING shadingMode	   = p_renderer->getShadingMode();

			ImGui::Begin( "Render passes" );
			ImGui::SetNextItemOpen( true );
			if ( ImGui::CollapsingHeader( "Geometric" ) ) {}
			ImGui::SetNextItemOpen( true );
			if ( ImGui::CollapsingHeader( "Linearize depth" ) ) {}
			ImGui::SetNextItemOpen( true );
			if ( ImGui::CollapsingHeader( "SSAO + Blur" ) )
			{
				if ( ImGui::Checkbox( "Active##SSAO", &activeSSAO ) )
				{
					p_renderer->setActiveSSAO( activeSSAO );
				}
				if ( ImGui::InputFloat( "Intensity", &ssaoIntensity ) )
				{
					p_renderer->setSSAOIntensity( ssaoIntensity );
				}
				if ( ImGui::InputFloat( "Blur size", &blurSize ) )
				{
					p_renderer->setBlurSize( blurSize );
				}
			}
			ImGui::SetNextItemOpen( true );
			if ( ImGui::CollapsingHeader( "Shading" ) )
			{
				const char * shadings[] = { "DIFFUSE", "GLOSSY", "TOON", "FLAT" };
				if ( ImGui::Combo( "Mode", (int *)( &shadingMode ), shadings, IM_ARRAYSIZE( shadings ) ) )
				{
					p_renderer->setShadingMode( shadingMode );
				}
				if ( ImGui::InputFloat( "Specular factor", &specularFactor ) )
				{
					p_renderer->setSpecularFactor( specularFactor );
				}
				if ( ImGui::ColorEdit4( "Background", (float *)( &colorBG ) ) )
				{
					p_renderer->setColorBackground( colorBG );
				}
				if ( ImGui::ColorEdit4( "Light", (float *)( &colorLight ) ) )
				{
					p_renderer->setColorLight( colorLight );
				}
				if ( ImGui::InputFloat( "Fog near", &fogNear ) )
				{
					p_renderer->setFogNear( fogNear );
				}
				if ( ImGui::InputFloat( "Fog far", &fogFar ) )
				{
					p_renderer->setFogFar( fogFar );
				}
				if ( ImGui::InputFloat( "Fog density", &fogDensity ) )
				{
					p_renderer->setFogDensity( fogDensity );
				}
				if ( ImGui::ColorEdit4( "Fog color", (float *)( &colorFog ) ) )
				{
					p_renderer->setColorFog( colorFog );
				}
			}
			ImGui::SetNextItemOpen( true );
			if ( ImGui::CollapsingHeader( "Outline" ) )
			{
				if ( ImGui::Checkbox( "Active##Outline", &activeOutline ) )
				{
					p_renderer->setActiveOutline( activeOutline );
				}
				if ( ImGui::InputFloat( "Sensivity", &outlineSensivity ) )
				{
					p_renderer->setOutlineSensivity( outlineSensivity );
				}
				if ( ImGui::InputFloat( "Thickness", &outlineThickness ) )
				{
					p_renderer->setOutlineThickness( outlineThickness );
				}
				if ( ImGui::ColorEdit4( "Outline##Color", (float *)( &colorOutline ) ) )
				{
					p_renderer->setColorOutline( colorOutline );
				}
			}
			ImGui::SetNextItemOpen( true );
			if ( ImGui::CollapsingHeader( "Selection" ) )
			{
				if ( ImGui::ColorEdit4( "Selection##Color", (float *)( &colorSelection ) ) )
				{
					p_renderer->setColorSelection( colorSelection );
				}
			}
			ImGui::SetNextItemOpen( true );
			if ( ImGui::CollapsingHeader( "FXAA" ) )
			{
				if ( ImGui::Checkbox( "Active##FXAA", &activeFXAA ) )
				{
					p_renderer->setActiveFXAA( activeFXAA );
				}
			}
			ImGui::End();

			// Times.
			static std::array<float, 8> & times = p_renderer->getBenchTimes();
			static const char *			  labels[]
				= { "Geometric", "Linearize depth", "SSAO", "Blur", "Shading", "Outline", "Selection", "FXAA" };
			ImGui::Begin( "Times" );
			ImGui::PlotHistogram( "",
								  (float *)times.data(),
								  8,
								  0,
								  "Passes",
								  0.0f,
								  float( *std::max_element( times.begin(), times.end() ) ),
								  ImVec2( 0, 80 ) );
			ImGui::End();

			// Misc.
			static const uint64_t sdlFrequency = SDL_GetPerformanceFrequency();
			static uint64_t		  lastTime	   = 0;
			const uint64_t		  now		   = SDL_GetPerformanceCounter();
			const float			  deltaTime	   = float( double( now - lastTime ) / sdlFrequency );
			lastTime						   = now;

			ImGui::Begin( "Misc" );
			// ImGui::Checkbox( "Perspective", &isPerspective );
			ImGui::Text( fmt::format( "{} FPS", int( 1.f / deltaTime ) ).c_str() );
			ImGui::Text( fmt::format( "{} average FPS", int( ImGui::GetIO().Framerate ) ).c_str() );

			if ( ImGui::Checkbox( "Vertical sync", &_vsync ) )
			{
				setVSync( _vsync );
			}
			ImGui::End();

			// Render.
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
			SDL_GL_SwapWindow( _window );
		}

		bool getEvent( SDL_Event & p_event ) const
		{
			bool hasEvent = SDL_PollEvent( &p_event );
			if ( hasEvent )
			{
				ImGui_ImplSDL2_ProcessEvent( &p_event );
			}
			return hasEvent;
		}

	  private:
		SDL_Window *  _window	 = nullptr;
		SDL_GLContext _glContext = nullptr;
		bool		  _vsync	 = false;
	}; // namespace VTX::Bench
} // namespace VTX::Bench
#endif
