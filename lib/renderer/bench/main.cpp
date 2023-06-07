#include "ui.hpp"
#include "util.hpp"
#include <iostream>
#include <renderer/gl/opengl_renderer.hpp>
#include <util/math.hpp>

constexpr size_t WIDTH	= 800;
constexpr size_t HEIGHT = 600;

int main( int, char ** )
{
	Util::Logger::get().init( std::filesystem::current_path() / "logs" );

	try
	{
		UI							 ui( WIDTH, HEIGHT );
		Renderer::GL::OpenGLRenderer renderer( ui.getProcAddress(), std::filesystem::current_path() / "shaders" );
		renderer.init( WIDTH, HEIGHT );

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
