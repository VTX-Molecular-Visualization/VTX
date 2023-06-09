#include "camera.hpp"
#include "user_interface.hpp"
#include "util.hpp"
#include <iostream>
#include <renderer/gl/opengl_renderer.hpp>
#include <util/math.hpp>

constexpr size_t WIDTH	= 1920;
constexpr size_t HEIGHT = 1200;

int main( int, char ** )
{
	using namespace VTX;
	Util::Logger::get().init( std::filesystem::current_path() / "logs" );

	try
	{
		using namespace VTX::Bench;
		using namespace VTX::Renderer::GL;

		// UI.
		UserInterface ui( WIDTH, HEIGHT );

		// Renderer.
		OpenGLRenderer renderer( ui.getProcAddress(), std::filesystem::current_path() / "shaders" );
		renderer.init( WIDTH, HEIGHT );

		// Camera.
		Camera camera( WIDTH, HEIGHT );
		camera.setCallbackMatrixView( [ &renderer ]( const Mat4f & p_matrix ) { renderer.setMatrixView( p_matrix ); } );
		camera.setCallbackMatrixProjection( [ &renderer ]( const Mat4f & p_matrix )
											{ renderer.setMatrixProjection( p_matrix ); } );

		// Resize.
		ui.setCallbackResize(
			[ &renderer, &camera ]( size_t p_width, size_t p_height )
			{
				renderer.resize( p_width, p_height );
				camera.resize( p_width, p_height );
			} );

		// Sample data.
		Core::Gpu::Mesh mesh	  = DEFAULT_MESH;
		StructProxyMesh proxyMesh = { &mesh.transform,	  &mesh.vertices,	&mesh.normals, &mesh.colors,
									  &mesh.visibilities, &mesh.selections, &mesh.ids,	   &mesh.indices };
		// renderer.addMesh( proxyMesh );

		// Core::Gpu::Molecule molecule = DEFAULT_MOLECULE;
		Core::Gpu::Molecule molecule = generateAtomGrid( 50 );
		StructProxyMolecule proxyMolecule
			= { &molecule.transform,		&molecule.atomPositions,  &molecule.atomColors, &molecule.atomRadii,
				&molecule.atomVisibilities, &molecule.atomSelections, &molecule.atomIds,	&molecule.bonds };
		renderer.addMolecule( proxyMolecule );

		while ( ui.shouldClose() == 0 )
		{
			float time = float( ui.getTime() );

			// TODO: move in a dedicated SSBO.
			Mat4f modelMatrix = Util::Math::rotate( MAT4F_ID, time * 0.01f, VEC3F_X );
			modelMatrix		  = Util::Math::rotate( modelMatrix, time * 0.02f, VEC3F_Y );
			modelMatrix		  = Util::Math::rotate( modelMatrix, time * 0.005f, VEC3F_Z );
			renderer.setMatrixModelTmp( modelMatrix );

			renderer.renderFrame();
			ui.draw();
		}
	}
	catch ( const std::exception & p_e )
	{
		VTX_ERROR( "{}", p_e.what() );
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
