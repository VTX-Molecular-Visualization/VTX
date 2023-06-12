#ifndef __VTX_BENCH_USER_INTERFACE__
#define __VTX_BENCH_USER_INTERFACE__

#define GLFW_INCLUDE_NONE
#include "camera.hpp"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
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
			glfwSwapInterval( 1 );

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

		void draw( Camera * const p_camera )
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// Camera.
			float		near		  = p_camera->getNear();
			float		far			  = p_camera->getFar();
			float		fov			  = p_camera->getFov();
			static bool isPerspective = true;
			ImGui::Begin( "Camera" );
			ImGui::Checkbox( "Perspective", &isPerspective );
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
			ImGui::Begin( "Render passes" );
			ImGui::SetNextItemOpen( true );
			if ( ImGui::CollapsingHeader( "Geometric" ) ) {}
			ImGui::SetNextItemOpen( true );
			if ( ImGui::CollapsingHeader( "Linearize depth" ) ) {}
			ImGui::SetNextItemOpen( true );
			if ( ImGui::CollapsingHeader( "SSAO" ) ) {}
			ImGui::SetNextItemOpen( true );
			if ( ImGui::CollapsingHeader( "Blur #1" ) ) {}
			ImGui::SetNextItemOpen( true );
			if ( ImGui::CollapsingHeader( "Blur #2" ) ) {}
			ImGui::SetNextItemOpen( true );
			if ( ImGui::CollapsingHeader( "Shading" ) ) {}
			ImGui::SetNextItemOpen( true );
			if ( ImGui::CollapsingHeader( "Outline" ) ) {}
			ImGui::SetNextItemOpen( true );
			if ( ImGui::CollapsingHeader( "Selection" ) ) {}
			ImGui::SetNextItemOpen( true );
			if ( ImGui::CollapsingHeader( "FXAA" ) ) {}

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

		static void _glfwErrorCallback( int p_error, const char * p_description )
		{
			VTX::VTX_ERROR( "GLFW Error {}: {}", p_error, p_description );
		}
	}; // namespace VTX::Bench
} // namespace VTX::Bench
#endif
