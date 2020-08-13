#include "box.hpp"
#include "util/math.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			void Box::createProgram()
			{
				Renderer::GLSL::ProgramManager & pm = VTXApp::get().getProgramManager();

				_program = pm.createProgram( "LineShader", { "line.vert", "line.frag" } );
			}

			void Box::setUniFormLocations()
			{
				assert( _program != nullptr );
				_uModelViewMatrixLoc = glGetUniformLocation( _program->getId(), "uMVMatrix" );
				_uProjMatrixLoc		 = glGetUniformLocation( _program->getId(), "uProjMatrix" );
			}

			void Box::render( const Generic::REPRESENTATION )
			{
				_program->use();

				// TODO: do not upadte each frame !
				const Object3D::Camera & cam = VTXApp::get().getScene().getCamera();
				glUniformMatrix4fv( _uModelViewMatrixLoc,
									1,
									GL_FALSE,
									Util::Math::value_ptr( cam.getViewMatrix() * _model->getTransform().get() ) );
				glUniformMatrix4fv( _uProjMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( cam.getProjectionMatrix() ) );
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
