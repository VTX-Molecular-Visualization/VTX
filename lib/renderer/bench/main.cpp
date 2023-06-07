#include <renderer/gl/opengl_renderer.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>
#include <renderer/gl/struct_mesh.hpp>
#include <renderer/gl/struct_molecule.hpp>
#include <util/logger.hpp>
#include <util/math.hpp>

constexpr size_t WIDTH	= 800;
constexpr size_t HEIGHT = 600;

using namespace VTX;

static void glfwErrorCallback( int p_error, const char * p_description )
{
	VTX::VTX_ERROR( "GLFW Error {}: {}", p_error, p_description );
}

const Core::StructMesh DEFAULT_MESH
	= { MAT4F_ID,
		{ Vec3f( 0.5f, -0.5f, 0.f ), Vec3f( -0.5f, -0.5f, 0.f ), Vec3f( 0.f, 0.5f, 0.f ) },
		{ Vec3f( 0.f, 0.f, 1.f ), Vec3f( 0.f, 0.f, 1.f ), Vec3f( 0.f, 0.f, 1.f ) },
		{ Util::Color::Rgba( 1.f, 0.f, 0.f, 1.f ),
		  Util::Color::Rgba( 0.f, 1.f, 0.f, 1.f ),
		  Util::Color::Rgba( 0.f, 0.f, 1.f, 1.f ) },
		{ 1, 1, 1, 1 },
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 0 },
		{ 0, 1, 2 } };

const Core::StructMolecule DEFAULT_MOLECULE
	= { MAT4F_ID,
		{ Vec3f( -2.f, 0.f, 0.f ), Vec3f( 2.f, 0.f, 0.f ), Vec3f( 0.f, 1.f, 0.f ) },
		{ Util::Color::Rgba::random(), Util::Color::Rgba::random(), Util::Color::Rgba::random() },
		{ 0.5f, 0.3f, 0.4 },
		{ 1, 1, 1 },
		{ 0, 0, 0 },
		{ 0, 0, 0 },
		{ 0, 1, 1, 2, 2, 0 }

	  };

Core::StructMolecule generateAtomGrid( int p_size )
{
	if ( p_size % 2 == 0 )
		++p_size;

	const size_t realSize = p_size * p_size * p_size;

	std::vector<Vec3f>			   positions( realSize );
	std::vector<Util::Color::Rgba> colors( realSize );

	size_t		counter = 0;
	const float offset	= 2.f;

	for ( int i = -p_size / 2; i <= p_size / 2; ++i )
	{
		for ( int j = -p_size / 2; j <= p_size / 2; ++j )
		{
			for ( int k = -p_size / 2; k <= p_size / 2; ++k )
			{
				positions[ counter ] = Vec3f( i * offset, j * offset, k * offset );
				colors[ counter ]	 = Util::Color::Rgba::random();
				++counter;
			}
		}
	}

	VTX_INFO( "{} atoms generated", realSize );

	return { MAT4F_ID,
			 positions,
			 colors,
			 std::vector<float>( realSize, 0.5f ),
			 std::vector<uint>( realSize, 1 ),
			 std::vector<uint>( realSize, 0 ),
			 std::vector<uint>( realSize, 0 ),
			 {} };
}

int main( int, char ** )
{
	Util::Logger::get().init( std::filesystem::current_path() / "logs" );

	glfwSetErrorCallback( glfwErrorCallback );
	if ( glfwInit() == 0 )
	{
		return EXIT_FAILURE;
	}

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
#if ( VTX_OPENGL_VERSION == 450 )
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
#else
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
#endif
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
		static auto renderer = Renderer::GL::OpenGLRenderer( reinterpret_cast<void *>( glfwGetProcAddress ),
															 std::filesystem::current_path() / "shaders" );
		renderer.init( WIDTH, HEIGHT );

		GLFWframebuffersizefun fun = []( GLFWwindow * p_window, int p_width, int p_height )
		{
			Mat4f projectionMatrix = Util::Math::perspective(
				Util::Math::radians( 60.f ), float( p_width ) / float( p_height ), 0.0001f, 1e4f );
			renderer.setMatrixProjection( projectionMatrix );
			renderer.resize( p_width, p_height );
		};
		glfwSetFramebufferSizeCallback( window, fun );

		// Camera.
		Vec3f positionCamera = Vec3f( 0.f, 0.f, 100.f );
		Mat4f matrixView	 = Util::Math::lookAt( positionCamera, positionCamera - VEC3F_Z, VEC3F_Y );
		renderer.setMatrixView( matrixView );
		Mat4f projectionMatrix
			= Util::Math::perspective( Util::Math::radians( 60.f ), float( WIDTH ) / float( HEIGHT ), 0.0001f, 1e4f );
		renderer.setMatrixProjection( projectionMatrix );
		auto bgColor = Util::Color::Rgba::randomPastel();
		renderer.setBackgroundColor( bgColor );

		// Sample data.
		Core::StructMesh mesh = DEFAULT_MESH;
		auto			 proxyMesh
			= Renderer::GL::StructProxyMesh { &mesh.tranform,	  &mesh.vertices,	&mesh.normals, &mesh.colors,
											  &mesh.visibilities, &mesh.selections, &mesh.ids,	   &mesh.indices };
		// renderer.addMesh( proxyMesh );

		// Core::StructMolecule molecule = DEFAULT_MOLECULE;
		Core::StructMolecule molecule = generateAtomGrid( 50 );
		auto proxyMolecule = Renderer::GL::StructProxyMolecule { &molecule.tranform,		 &molecule.atomPositions,
																 &molecule.atomColors,		 &molecule.atomRadii,
																 &molecule.atomVisibilities, &molecule.atomSelections,
																 &molecule.atomIds,			 &molecule.bonds };
		renderer.addMolecule( proxyMolecule );
		while ( glfwWindowShouldClose( window ) == 0 )
		{
			float time = float( glfwGetTime() );
			// TODO: move in a dedicated SSBO.
			Mat4f modelMatrix = Util::Math::rotate( MAT4F_ID, time * 0.01f, VEC3F_X );
			modelMatrix		  = Util::Math::rotate( modelMatrix, time * 0.02f, VEC3F_Y );
			modelMatrix		  = Util::Math::rotate( modelMatrix, time * 0.005f, VEC3F_Z );
			renderer.setMatrixModelTmp( modelMatrix );
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
		VTX_ERROR( "Exception: {}", p_e.what() );
		glfwDestroyWindow( window );
		glfwTerminate();
		return EXIT_FAILURE;
	}
}
