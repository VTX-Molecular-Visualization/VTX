#include "box.hpp"
#include "util/math.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			Box::Box( Model::Molecule * const p_model ) : BaseView3DMolecule( p_model )
			{
				Renderer::GLSL::ProgramManager & pm = VTXApp::get().getProgramManager();

				_program = pm.createProgram( "LineShader", { "line.vert", "line.frag" } );

				assert( _program != nullptr );
				_uModelViewMatrixLoc = OGL().glGetUniformLocation( _program->getId(), "uMVMatrix" );
				_uProjMatrixLoc		 = OGL().glGetUniformLocation( _program->getId(), "uProjMatrix" );
			}

			void Box::render( const Generic::REPRESENTATION )
			{
				_program->use();

				// TODO: do not upadte each frame !
				const Object3D::Camera & cam = VTXApp::get().getScene().getCamera();
				OGL().glUniformMatrix4fv( _uModelViewMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( cam.getViewMatrix() * _model->getTransform().get() ) );
				OGL().glUniformMatrix4fv( _uProjMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( cam.getProjectionMatrix() ) );
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
