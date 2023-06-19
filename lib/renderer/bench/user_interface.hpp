#ifndef __VTX_BENCH_USER_INTERFACE__
#define __VTX_BENCH_USER_INTERFACE__

#define GLFW_INCLUDE_NONE
#include "camera.hpp"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <renderer/gl/opengl_renderer.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>

namespace VTX::Bench
{
	// TODO: dirty.
	static std::function<void( const size_t, const size_t )> _callbackResize;

	class UserInterface
	{
	  public:
		UserInterface( const size_t p_width, const size_t p_height )
		{
			// Init GLFW.
			glfwSetErrorCallback( _glfwErrorCallback );
			if ( glfwInit() == 0 )
			{
				throw std::runtime_error( "Failed to init GLFW" );
			}

			glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
			glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
			glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

			_window = glfwCreateWindow( int( p_width ), int( p_height ), "VTX_RENDERER_BENCH", nullptr, nullptr );

			if ( _window == nullptr )
			{
				throw std::runtime_error( "Failed to create GLFW window" );
			}

			glfwMakeContextCurrent( _window );
			glfwSwapInterval( _vsync );

			// Init ImGui.
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGui::StyleColorsDark();
			ImGui_ImplGlfw_InitForOpenGL( _window, true );
			ImGui_ImplOpenGL3_Init( "#version 450 core" );

			// Resize.
			glfwSetFramebufferSizeCallback( _window,
											[]( GLFWwindow * p_window, int p_width, int p_height )
											{
												if ( _callbackResize )
												{
													_callbackResize( p_width, p_height );
												}
											} );
		}

		~UserInterface()
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
			glfwDestroyWindow( _window );
			glfwTerminate();
		}

		inline double getTime() const { return glfwGetTime(); }
		inline float  getDeltaTime() const { return ImGui::GetIO().DeltaTime; }
		inline bool	  shouldClose() const { return glfwWindowShouldClose( _window ); }
		inline void * getProcAddress() { return reinterpret_cast<void *>( glfwGetProcAddress ); }
		inline void	  setVSync( const bool p_vsync )
		{
			_vsync = p_vsync;
			glfwSwapInterval( _vsync );
		}

		inline Vec3i consumeMoveInputs()
		{
			Vec3i deltaMoveInputs = _deltaMoveInputs;
			_deltaMoveInputs	  = Vec3i( 0, 0, 0 );
			return deltaMoveInputs;
		}

		void setCallbackResize( std::function<void( const size_t, const size_t )> p_callback )
		{
			_callbackResize = p_callback;
		}

		void draw( Renderer::GL::OpenGLRenderer * const p_renderer, Camera * const p_camera )
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

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

			// Misc.
			ImGui::Begin( "Misc" );
			// ImGui::Checkbox( "Perspective", &isPerspective );
			if ( ImGui::Checkbox( "Vertical sync", &_vsync ) )
			{
				setVSync( _vsync );
			}
			ImGui::End();

			// Times.
			static std::array<float, 8> & times = p_renderer->getBenchTimes();
			static const char *			  labels[]
				= { "Geometric", "Linearize depth", "SSAO", "Blur", "Shading", "Outline", "Selection", "FXAA" };
			ImGui::Begin( "Times" );
			ImGui::PlotHistogram( "",
								  times.data(),
								  8,
								  0,
								  "Passes",
								  0.0f,
								  *std::max_element( times.begin(), times.end() ),
								  ImVec2( 0, 80 ) );
			ImGui::End();

			ImGui::Render();
			glfwSetWindowTitle(
				_window,
				( std::string( "VTX_RENDERER_BENCH " ) + std::to_string( int( ImGui::GetIO().Framerate ) ) + "FPS" )
					.c_str() );

			ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
			glfwSwapBuffers( _window );
			glfwPollEvents();
		}

		void processInputs()
		{
			if ( glfwGetKey( _window, GLFW_KEY_W ) == GLFW_PRESS )
			{
				_deltaMoveInputs.z++;
			}
			if ( glfwGetKey( _window, GLFW_KEY_S ) == GLFW_PRESS )
			{
				_deltaMoveInputs.z--;
			}
			if ( glfwGetKey( _window, GLFW_KEY_A ) == GLFW_PRESS )
			{
				_deltaMoveInputs.x--;
			}
			if ( glfwGetKey( _window, GLFW_KEY_D ) == GLFW_PRESS )
			{
				_deltaMoveInputs.x++;
			}
			if ( glfwGetKey( _window, GLFW_KEY_R ) == GLFW_PRESS )
			{
				_deltaMoveInputs.y++;
			}
			if ( glfwGetKey( _window, GLFW_KEY_F ) == GLFW_PRESS )
			{
				_deltaMoveInputs.y--;
			}
		}

	  private:
		GLFWwindow * _window;
		Vec3i		 _deltaMoveInputs;
		bool		 _vsync = true;

		static void _glfwErrorCallback( int p_error, const char * p_description )
		{
			VTX::VTX_ERROR( "GLFW Error {}: {}", p_error, p_description );
		}
	}; // namespace VTX::Bench
} // namespace VTX::Bench
#endif
