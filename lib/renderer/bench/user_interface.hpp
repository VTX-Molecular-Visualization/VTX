#ifndef __VTX_BENCH_USER_INTERFACE__
#define __VTX_BENCH_USER_INTERFACE__

#include "camera.hpp"
#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>
#include <imnodes.h>
#include <renderer/gl/opengl_renderer.hpp>
#include <renderer/renderer.hpp>
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

			// ImGui plugins.
			ImNodes::CreateContext();
		}

		~UserInterface()
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplSDL2_Shutdown();
			if ( ImGui::GetCurrentContext() != nullptr )
			{
				ImNodes::DestroyContext();
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

		void draw( Renderer::GL::OpenGLRenderer * const p_renderer,
				   Camera * const						p_camera,
				   Renderer::Renderer * const			p_newRenderer )
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();

			// Menu bar.
			if ( ImGui::BeginMainMenuBar() )
			{ // Main menu.
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
			_drawCamera( p_camera );

			// Passes.
			_drawPasses( p_renderer );

			// Times.
			_drawTimes( p_renderer );

			// Misc.
			_drawMisc( p_renderer );

			// Node editor.
			_drawNodeEditor( p_renderer, p_newRenderer );

			// Render.
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
			SDL_GL_SwapWindow( _window );
		}

		void _drawCamera( Camera * const p_camera ) const
		{
			if ( ImGui::Begin( "Camera" ) )
			{
				float near				  = p_camera->getNear();
				float far				  = p_camera->getFar();
				float fov				  = p_camera->getFov();
				float velocityTranslation = p_camera->getVelocityTranslation();
				float velocityRotation	  = p_camera->getVelocityRotation();
				float velocityZoom		  = p_camera->getVelocityZoom();
				// static bool	 isPerspective = true;

				// ImGui::Checkbox( "Perspective", &isPerspective );
				if ( ImGui::SliderFloat( "Near", &near, Camera::NEAR_MIN, Camera::NEAR_MAX ) )
				{
					p_camera->setNear( near );
				}
				if ( ImGui::SliderFloat( "Far", &far, Camera::FAR_MIN, Camera::FAR_MAX ) )
				{
					p_camera->setFar( far );
				}
				if ( ImGui::SliderFloat( "Fov", &fov, Camera::FOV_MIN, Camera::FOV_MAX ) )
				{
					p_camera->setFov( fov );
				}
				if ( ImGui::SliderFloat( "Velocity translation",
										 &velocityTranslation,
										 Camera::VELOCITY_TRANSLATION_MIN,
										 Camera::VELOCITY_TRANSLATION_MAX ) )
				{
					p_camera->setVelocityTranslation( velocityTranslation );
				}
				if ( ImGui::SliderFloat( "Velocity rotation",
										 &velocityRotation,
										 Camera::VELOCITY_ROTATION_MIN,
										 Camera::VELOCITY_ROTATION_MAX ) )
				{
					p_camera->setVelocityRotation( velocityRotation );
				}
				if ( ImGui::SliderFloat(
						 "Velocity zoom", &velocityZoom, Camera::VELOCITY_ZOOM_MIN, Camera::VELOCITY_ZOOM_MAX ) )
				{
					p_camera->setVelocityZoom( velocityZoom );
				}
			}
			ImGui::End();
		}

		void _drawPasses( Renderer::GL::OpenGLRenderer * const p_renderer ) const
		{
			if ( ImGui::Begin( "Render passes" ) )
			{
				bool activeSSAO		= p_renderer->isActiveSSAO();
				bool activeOutline	= p_renderer->isActiveOutline();
				bool activeFXAA		= p_renderer->isActiveFXAA();
				bool activePixelize = p_renderer->isActivePixelize();

				float ssaoIntensity = p_renderer->getSSAOIntensity();
				float blurSize		= p_renderer->getBlurSize();

				Renderer::GL::ENUM_SHADING shadingMode	  = p_renderer->getShadingMode();
				float					   specularFactor = p_renderer->getSpecularFactor();
				int						   toonSteps	  = p_renderer->getToonSteps();
				Util::Color::Rgba		   colorBG		  = p_renderer->getColorBackground();
				Util::Color::Rgba		   colorLight	  = p_renderer->getColorLight();
				Util::Color::Rgba		   colorFog		  = p_renderer->getColorFog();
				float					   fogNear		  = p_renderer->getFogNear();
				float					   fogFar		  = p_renderer->getFogFar();
				float					   fogDensity	  = p_renderer->getFogDensity();

				Util::Color::Rgba colorOutline	   = p_renderer->getColorOutline();
				float			  outlineSensivity = p_renderer->getOutlineSensivity();
				uint			  outlineThickness = p_renderer->getOutlineThickness();

				Util::Color::Rgba colorSelection = p_renderer->getColorSelection();

				int	 pixelSize			  = p_renderer->getPixelSize();
				bool pixelarizeBackground = p_renderer->isPixelizeBackground();

				static bool isOpenGeometric;
				if ( isOpenGeometric = ImGui::CollapsingHeader( "Geometric" ) ) {}
				static bool isOpenLinearizeDepth;
				if ( isOpenLinearizeDepth = ImGui::CollapsingHeader( "Linearize depth" ) ) {}
				static bool isOpenSSAO;
				if ( isOpenSSAO = ImGui::CollapsingHeader( "SSAO + Blur", ImGuiTreeNodeFlags_DefaultOpen ) )
				{
					if ( ImGui::Checkbox( "Active##SSAO", &activeSSAO ) )
					{
						p_renderer->setActiveSSAO( activeSSAO );
					}
					if ( ImGui::SliderFloat( "Intensity",
											 &ssaoIntensity,
											 Renderer::GL::Pass::SSAO::AO_INTENSITY_MIN,
											 Renderer::GL::Pass::SSAO::AO_INTENSITY_MAX ) )
					{
						p_renderer->setSSAOIntensity( ssaoIntensity );
					}
					if ( ImGui::SliderFloat( "Blur size",
											 &blurSize,
											 Renderer::GL::Pass::Blur::BLUR_SIZE_MIN,
											 Renderer::GL::Pass::Blur::BLUR_SIZE_MAX ) )
					{
						p_renderer->setBlurSize( blurSize );
					}
				}
				static bool isOpenShading;
				if ( isOpenShading = ImGui::CollapsingHeader( "Shading", ImGuiTreeNodeFlags_DefaultOpen ) )
				{
					const char * shadings[] = { "DIFFUSE", "GLOSSY", "TOON", "FLAT" };
					if ( ImGui::Combo( "Mode", (int *)( &shadingMode ), shadings, IM_ARRAYSIZE( shadings ) ) )
					{
						p_renderer->setShadingMode( shadingMode );
					}
					if ( shadingMode == Renderer::GL::ENUM_SHADING::GLOSSY )
					{
						if ( ImGui::SliderFloat( "Specular factor",
												 &specularFactor,
												 Renderer::GL::Pass::Shading::SPECULAR_FACTOR_MIN,
												 Renderer::GL::Pass::Shading::SPECULAR_FACTOR_MAX ) )
						{
							p_renderer->setSpecularFactor( specularFactor );
						}
					}
					else if ( shadingMode == Renderer::GL::ENUM_SHADING::TOON )
					{
						if ( ImGui::SliderInt( "Steps",
											   &toonSteps,
											   Renderer::GL::Pass::Shading::TOON_STEPS_MIN,
											   Renderer::GL::Pass::Shading::TOON_STEPS_MAX ) )
						{
							p_renderer->setToonSteps( toonSteps );
						}
					}
					if ( ImGui::ColorEdit4( "Background", (float *)( &colorBG ) ) )
					{
						p_renderer->setColorBackground( colorBG );
					}
					if ( ImGui::ColorEdit4( "Light", (float *)( &colorLight ) ) )
					{
						p_renderer->setColorLight( colorLight );
					}
					if ( ImGui::SliderFloat( "Fog near",
											 &fogNear,
											 Renderer::GL::Pass::Shading::FOG_NEAR_MIN,
											 Renderer::GL::Pass::Shading::FOG_NEAR_MAX ) )
					{
						p_renderer->setFogNear( fogNear );
					}
					if ( ImGui::SliderFloat( "Fog far",
											 &fogFar,
											 Renderer::GL::Pass::Shading::FOG_FAR_MIN,
											 Renderer::GL::Pass::Shading::FOG_FAR_MAX ) )
					{
						p_renderer->setFogFar( fogFar );
					}
					if ( ImGui::SliderFloat( "Fog density",
											 &fogDensity,
											 Renderer::GL::Pass::Shading::FOG_DENSITY_MIN,
											 Renderer::GL::Pass::Shading::FOG_DENSITY_MAX ) )
					{
						p_renderer->setFogDensity( fogDensity );
					}
					if ( ImGui::ColorEdit4( "Fog color", (float *)( &colorFog ) ) )
					{
						p_renderer->setColorFog( colorFog );
					}
				}
				static bool isOpenOutline;
				if ( isOpenOutline = ImGui::CollapsingHeader( "Outline", ImGuiTreeNodeFlags_DefaultOpen ) )
				{
					if ( ImGui::Checkbox( "Active##Outline", &activeOutline ) )
					{
						p_renderer->setActiveOutline( activeOutline );
					}
					if ( ImGui::SliderFloat( "Sensivity",
											 &outlineSensivity,
											 Renderer::GL::Pass::Outline::OUTLINE_SENSIVITY_MIN,
											 Renderer::GL::Pass::Outline::OUTLINE_SENSIVITY_MAX ) )
					{
						p_renderer->setOutlineSensivity( outlineSensivity );
					}
					if ( ImGui::SliderInt( "Thickness",
										   (int *)( &outlineThickness ),
										   Renderer::GL::Pass::Outline::OUTLINE_THICKNESS_MIN,
										   Renderer::GL::Pass::Outline::OUTLINE_THICKNESS_MAX ) )
					{
						p_renderer->setOutlineThickness( outlineThickness );
					}
					if ( ImGui::ColorEdit4( "Color##Outline", (float *)( &colorOutline ) ) )
					{
						p_renderer->setColorOutline( colorOutline );
					}
				}
				static bool isOpenSelection;
				if ( isOpenSelection = ImGui::CollapsingHeader( "Selection" ) )
				{
					if ( ImGui::ColorEdit4( "Color##Selection", (float *)( &colorSelection ) ) )
					{
						p_renderer->setColorSelection( colorSelection );
					}
				}
				static bool isOpenFXAA;
				if ( isOpenFXAA = ImGui::CollapsingHeader( "FXAA" ) )
				{
					if ( ImGui::Checkbox( "Active##FXAA", &activeFXAA ) )
					{
						p_renderer->setActiveFXAA( activeFXAA );
					}
				}
				static bool isOpenPixelize;
				if ( isOpenPixelize = ImGui::CollapsingHeader( "Pixelize" ) )
				{
					if ( ImGui::Checkbox( "Active##Pixelize", &activePixelize ) )
					{
						p_renderer->setActivePixelize( activePixelize );
					}
					if ( ImGui::SliderInt( "Pixel size", &pixelSize, 1, 151 ) )
					{
						p_renderer->setPixelSize( pixelSize );
					}
					if ( ImGui::Checkbox( "Background?", &pixelarizeBackground ) )
					{
						p_renderer->setPixelizeBackground( pixelarizeBackground );
					}
				}
			}
			ImGui::End();
		}

		void _drawTimes( Renderer::GL::OpenGLRenderer * const p_renderer ) const
		{
			bool isTimersEnabled = p_renderer->isTimersEnabled();
			if ( isTimersEnabled )
			{
				auto &				times	 = p_renderer->getTimes();
				static const char * labels[] = { "Geometric", "Linearize depth", "SSAO", "Blur",	 "Shading",
												 "Outline",	  "Selection",		 "FXAA", "Pixelize", "Blit FBO" };
				if ( ImGui::Begin( "Times (ms)" ) )
				{
					const float max = *std::max_element( times.begin(), times.end() );
					for ( size_t i = 0; i < times.size(); ++i )
					{
						ImGui::ProgressBar(
							times[ i ] / max, ImVec2( 0.f, 0.f ), std::to_string( times[ i ] ).c_str() );
						ImGui::SameLine( 0.0f, ImGui::GetStyle().ItemInnerSpacing.x );
						ImGui::Text( labels[ i ] );
					}
				}
				ImGui::End();
			}
		}

		void _drawMisc( Renderer::GL::OpenGLRenderer * const p_renderer )
		{
			static const uint64_t sdlFrequency					= SDL_GetPerformanceFrequency();
			static uint64_t		  lastTime						= 0;
			const uint64_t		  now							= SDL_GetPerformanceCounter();
			const float			  deltaTime						= float( double( now - lastTime ) / sdlFrequency );
			lastTime											= now;
			const Renderer::GL::StructOpenglInfos & openglInfos = p_renderer->getOpenglInfos();

			if ( ImGui::Begin( "Misc" ) )
			{
				// ImGui::Checkbox( "Perspective", &isPerspective );
				bool isTimersEnabled = p_renderer->isTimersEnabled();
				ImGui::Text( fmt::format( "{} FPS", int( 1.f / deltaTime ) ).c_str() );
				ImGui::Text( fmt::format( "{} average FPS", int( ImGui::GetIO().Framerate ) ).c_str() );

				ImGui::ProgressBar( float( ( openglInfos.gpuMemoryInfoTotalAvailableMemoryNVX
											 - openglInfos.gpuMemoryInfoCurrentAvailableVidMemNVX ) )
										/ openglInfos.gpuMemoryInfoTotalAvailableMemoryNVX,
									ImVec2( 0.f, 0.f ),
									fmt::format( "{} / {}",
												 ( openglInfos.gpuMemoryInfoTotalAvailableMemoryNVX
												   - openglInfos.gpuMemoryInfoCurrentAvailableVidMemNVX ),
												 openglInfos.gpuMemoryInfoTotalAvailableMemoryNVX )
										.c_str() );
				ImGui::SameLine( 0.0f, ImGui::GetStyle().ItemInnerSpacing.x );
				ImGui::Text( "GPU memory" );

				if ( ImGui::Checkbox( "Vertical sync", &_vsync ) )
				{
					setVSync( _vsync );
				}
				if ( ImGui::Checkbox( "Enable timers", &isTimersEnabled ) )
				{
					p_renderer->setTimersEnabled( isTimersEnabled );
				}
			}
			ImGui::End();
		}

		void _drawNodeEditor( Renderer::GL::OpenGLRenderer * const p_renderer,
							  Renderer::Renderer * const		   p_newRenderer ) const
		{
			if ( ImGui::Begin( "Render graph" ) )
			{
				ImNodes::BeginNodeEditor();

				// Passes.
				uint												 id = 0;
				std::map<const Renderer::Pass::Input * const, uint>	 mapInputId;
				std::map<const Renderer::Pass::Output * const, uint> mapOutputId;
				for ( const auto & [ name, pass ] : p_newRenderer->getRenderGraph().getPasses() )
				{
					ImNodes::BeginNode( id++ );
					ImNodes::BeginNodeTitleBar();
					ImGui::TextUnformatted( name.c_str() );
					ImNodes::EndNodeTitleBar();

					// Inputs.
					for ( const auto & [ channel, input ] : pass.inputs )
					{
						mapInputId.emplace( &input, id );
						ImNodes::BeginInputAttribute( id++ );
						ImGui::Text( std::to_string( uint( channel ) ).c_str() );
						ImNodes::EndInputAttribute();
					}

					// Output.
					mapOutputId.emplace( &pass.output, id );
					ImNodes::BeginOutputAttribute( id++ );
					ImGui::Text( "out" );
					ImNodes::EndOutputAttribute();
					ImNodes::EndNode();
				}

				// Links.
				for ( const auto & link : p_newRenderer->getRenderGraph().getLinks() )
				{
					ImNodes::Link( id++,
								   mapOutputId[ &( link.source->output ) ],
								   mapInputId[ &( link.destination->inputs[ link.channel ] ) ] );
				}

				ImNodes::MiniMap();
				ImNodes::EndNodeEditor();
			}
			ImGui::End();
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
