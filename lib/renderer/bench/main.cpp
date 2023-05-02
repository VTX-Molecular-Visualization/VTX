#include <renderer/gl/opengl_renderer.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>
#include <util/logger.hpp>

constexpr size_t WIDTH	= 800;
constexpr size_t HEIGHT = 600;

int main( int argc, char ** argv )
{
	VTX::Util::Logger::get().init( std::filesystem::current_path() / "logs" );

	if ( glfwInit() == 0 )
	{
		exit( EXIT_FAILURE );
	}

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	GLFWwindow * const window = glfwCreateWindow( WIDTH, HEIGHT, "VTX_RENDERER_BENCH", NULL, NULL );
	if ( window == nullptr )
	{
		VTX::VTX_ERROR( "Failed to create GLFW window" );
		glfwTerminate();
		exit( EXIT_FAILURE );
	}

	glfwMakeContextCurrent( window );

	try
	{
		VTX::Renderer::GL::OpenGLRenderer renderer( glfwGetProcAddress );
		renderer.init( WIDTH, HEIGHT );

		// glViewport( 0, 0, WIDTH, HEIGHT );
		// glClearColor( 0.5f, 0.5f, 0.5f, 1.f );

		while ( glfwWindowShouldClose( window ) == 0 )
		{
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
			glfwSwapBuffers( window );
			glfwPollEvents();
		}

		glfwDestroyWindow( window );
		glfwTerminate();
		exit( EXIT_SUCCESS );
	}
	catch ( const std::exception & p_e )
	{
		VTX::VTX_ERROR( p_e.what() );
		glfwTerminate();
		exit( EXIT_FAILURE );
	}
}
