#include <renderer/gl/opengl_renderer.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>
#include <util/logger.hpp>
#include <util/math.hpp>

constexpr size_t WIDTH	= 800;
constexpr size_t HEIGHT = 600;

static void glfwErrorCallback( int p_error, const char * p_description )
{
	VTX::VTX_ERROR( "GLFW Error {}: {}", p_error, p_description );
}

int main( int, char ** )
{
	using namespace VTX;
	Util::Logger::get().init( std::filesystem::current_path() / "logs" );

	glfwSetErrorCallback( glfwErrorCallback );
	if ( glfwInit() == 0 )
	{
		return EXIT_FAILURE;
	}

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	GLFWwindow * const window = glfwCreateWindow( WIDTH, HEIGHT, "VTX_RENDERER_BENCH", nullptr, nullptr );

	if ( window == nullptr )
	{
		VTX_ERROR( "Failed to create GLFW window" );
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent( window );
	// glfwSwapInterval( 1 );

	try
	{
		// Renderer.
		auto renderer = Renderer::GL::OpenGLRenderer( static_cast<void *>( glfwGetProcAddress ),
													  std::filesystem::current_path() / "shaders" );
		renderer.init( WIDTH, HEIGHT );

		/*
		GLFWframebuffersizefun fun = [ &renderer ]( GLFWwindow * p_window, int p_width, int p_height )
		{ renderer.resize( p_width, p_height ); };
		glfwSetFramebufferSizeCallback( window, fun );
		*/

		// Camera.
		Vec3f position	 = Vec3f( 0.f, 0.f, 2.f );
		Mat4f viewMatrix = Util::Math::lookAt( position, position - VEC3F_Z, VEC3F_Y );
		Mat4f projectionMatrix
			= Util::Math::perspective( Util::Math::radians( 60.f ), WIDTH / float( HEIGHT ), 0.0001f, 1000.f );

		renderer.setCameraMatrix( viewMatrix, projectionMatrix );
		// renderer.setBackgroundColor( Util::Color::Rgba( 1.f, 0.f, 0.f, 1.f ) );

		while ( glfwWindowShouldClose( window ) == 0 )
		{
			// glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
			renderer.renderFrame();
			glfwSwapBuffers( window );
			glfwPollEvents();
		}

		glfwDestroyWindow( window );
		glfwTerminate();
		return EXIT_SUCCESS;
	}
	catch ( const std::exception & p_e )
	{
		VTX_ERROR( p_e.what() );
		glfwDestroyWindow( window );
		glfwTerminate();
		return EXIT_FAILURE;
	}
}
