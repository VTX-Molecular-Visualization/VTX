#include "triangle.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			Triangle::Triangle( Model::MeshTriangle * const p_model ) : BaseView3D( p_model )
			{
				Renderer::GLSL::ProgramManager & pm = VTXApp::get().getProgramManager();
				_program							= pm.createProgram( "Triangle", { "triangle.vert", "triangle.frag" } );

				assert( _program != nullptr );
				_uModelViewMatrixLoc = _gl()->glGetUniformLocation( _program->getId(), "uMVMatrix" );
				_uProjMatrixLoc		 = _gl()->glGetUniformLocation( _program->getId(), "uProjMatrix" );
				_uNormalMatrixLoc	 = _gl()->glGetUniformLocation( _program->getId(), "uNormalMatrix" );
			}

			void Triangle::render()
			{
				_program->use();

				// TODO: do not upadte each frame !
				const Object3D::Camera & cam	  = VTXApp::get().getScene().getCamera();
				const Mat4f				 MVMatrix = cam.getViewMatrix() * _model->getTransform().get();
				_gl()->glUniformMatrix4fv( _uModelViewMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( MVMatrix ) );
				_gl()->glUniformMatrix4fv( _uProjMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( cam.getProjectionMatrix() ) );
				_gl()->glUniformMatrix4fv( _uNormalMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( Util::Math::transpose( Util::Math::inverse( MVMatrix ) ) ) );

				_gl()->glDrawElements( GL_TRIANGLES, uint( _model->getIndices().size() ), GL_UNSIGNED_INT, 0 );
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
