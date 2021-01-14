#include "box.hpp"
#include "util/math.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			Box::Box( Model::BaseModel3D<Buffer::BaseBufferOpenGL> * const p_model ) : BaseView3D( p_model )
			{
				Renderer::GLSL::ProgramManager & pm = VTXApp::get().getProgramManager();

				_program = pm.createProgram( "LineShader", { "line.vert", "line.frag" } );

				assert( _program != nullptr );
				_uModelViewMatrixLoc = _gl()->glGetUniformLocation( _program->getId(), "uMVMatrix" );
				_uProjMatrixLoc		 = _gl()->glGetUniformLocation( _program->getId(), "uProjMatrix" );
				_uNormalMatrixLoc	 = _gl()->glGetUniformLocation( _program->getId(), "uNormalMatrix" );
			}

			void Box::render()
			{
				_program->use();

				const Object3D::Camera & cam = VTXApp::get().getScene().getCamera();
				_gl()->glUniformMatrix4fv( _uModelViewMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( cam.getViewMatrix() * _model->getTransform().get() ) );
				_gl()->glUniformMatrix4fv( _uProjMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( cam.getProjectionMatrix() ) );
				_gl()->glUniformMatrix4fv(
					_uNormalMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( Util::Math::transpose( Util::Math::inverse( cam.getViewMatrix() * _model->getTransform().get() ) ) ) );
				GLfloat width = 2.f;
				_gl()->glLineWidth( width );
				_gl()->glDrawElements( GL_LINES, uint( _model->getBufferAABBIndices().size() ), GL_UNSIGNED_INT, 0 );
			}

		} // namespace D3
	}	  // namespace View
} // namespace VTX
