#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>

void errorCallback( int error, const char * p_description ) { std::cerr << p_description << std::endl; }

int main( int argc, char ** argv )
{
	if ( !glfwInit() )
	{
		exit( EXIT_FAILURE );
	}

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	GLFWwindow * const window = glfwCreateWindow( 800, 600, "VTX_RENDERER_BENCH", NULL, NULL );
	if ( !window )
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit( EXIT_FAILURE );
	}

	glfwMakeContextCurrent( window );
	if ( !gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress ) )
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return EXIT_FAILURE;
	}

	const unsigned char * glVendor	  = glGetString( GL_VENDOR );
	const unsigned char * glRenderer  = glGetString( GL_RENDERER );
	const unsigned char * glVersion	  = glGetString( GL_VERSION );
	const unsigned char * glslVersion = glGetString( GL_SHADING_LANGUAGE_VERSION );

	std::cout << "GL device: " << glVendor << " " << glRenderer << std::endl;
	std::cout << "GL version: " << glVersion << std::endl;
	std::cout << "GLSL version: " << glslVersion << std::endl;
	std::cout << "GLAD initialized: " << GLVersion.major << "." << GLVersion.minor << std::endl;

	glViewport( 0, 0, 800, 600 );
	glClearColor( 0.5f, 0.5f, 0.5f, 1.f );

	while ( !glfwWindowShouldClose( window ) )
	{
		glClear( GL_COLOR_BUFFER_BIT );
		glfwSwapBuffers( window );
		glfwPollEvents();
	}

	glfwDestroyWindow( window );

	glfwTerminate();
	exit( EXIT_SUCCESS );
}
