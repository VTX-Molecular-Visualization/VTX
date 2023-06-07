#ifndef __VTX_BENCH_UI__
#define __VTX_BENCH_UI__

#define GLFW_INCLUDE_NONE
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <util/logger.hpp>

class UI
{
  public:
	UI( const size_t p_width, const size_t p_height )
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
		/*
		GLFWframebuffersizefun fun = []( GLFWwindow * p_window, int p_width, int p_height )
		{
			Mat4f projectionMatrix = Util::Math::perspective(
				Util::Math::radians( 60.f ), float( p_width ) / float( p_height ), 0.0001f, 1e4f );
			renderer.setMatrixProjection( projectionMatrix );
			renderer.resize( p_width, p_height );
		};
		glfwSetFramebufferSizeCallback( _window, fun );
		*/
	}

	~UI()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		glfwDestroyWindow( _window );
		glfwTerminate();
	}

	inline double getTime() const { return glfwGetTime(); }
	inline bool	  shouldClose() const { return glfwWindowShouldClose( _window ); }
	inline void * getProcAddress() { return reinterpret_cast<void *>( glfwGetProcAddress ); }

	void draw()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin( "Render settings" );

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

  private:
	GLFWwindow * _window;
	// Function for resize callback.

	static void _glfwErrorCallback( int p_error, const char * p_description )
	{
		VTX::VTX_ERROR( "GLFW Error {}: {}", p_error, p_description );
	}
};

#endif
