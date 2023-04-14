#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>

void errorCallback( int error, const char * description ) { std::cerr << description << std::endl; }

int main( int argc, char ** argv )
{
	if ( !glfwInit() )
	{
		exit( EXIT_FAILURE );
	}

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	GLFWwindow * const window = glfwCreateWindow( 1920, 1080, "VTX_RENDERER_BENCH", NULL, NULL );
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
	std::cout << "GLAD initialized: " << GLVersion.major << "." << GLVersion.minor << std::endl;

	glViewport( 0, 0, 1920, 1080 );
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
