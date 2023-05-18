#include <catch2/catch_test_macros.hpp>
#include <renderer/gl/opengl_renderer.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>

TEST_CASE( "Renderer::GL", "[renderer]" )
{
	/*
	using namespace VTX;

	REQUIRE( glfwInit() );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	glfwWindowHint( GLFW_VISIBLE, GLFW_FALSE );

	GLFWwindow * const window = glfwCreateWindow( 800, 600, "VTX_RENDERER_TEST", nullptr, nullptr );

	REQUIRE( window );

	glfwMakeContextCurrent( window );

	try
	{
		auto renderer = Renderer::GL::OpenGLRenderer( reinterpret_cast<void *>( glfwGetProcAddress ),
													  std::filesystem::current_path() / "shaders" );
		renderer.init( 800, 600 );
		renderer.renderFrame();
		glfwSwapBuffers( window );
	}
	catch ( const std::exception & p_e )
	{
		std::cerr << p_e.what() << std::endl;
	}

	glfwDestroyWindow( window );
	glfwTerminate();
	*/
}
