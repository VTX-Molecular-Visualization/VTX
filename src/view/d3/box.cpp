#include "box.hpp"
#include "util/math.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			void Box::init()
			{
				Renderer::GLSL::ProgramManager & pm = VTXApp::get().getProgramManager();
				Renderer::GLSL::Program * program	= pm.createProgram( "LineShader", { "line.vert", "line.frag" } );

				_uModelViewMatrix = glGetUniformLocation( program->getId(), "uMVMatrix" );
				_uProjMatrix	  = glGetUniformLocation( program->getId(), "uProjMatrix" );
			}

			void Box::render( const Generic::REPRESENTATION )
			{
				VTXApp::get().getProgramManager().getProgram( "LineShader" )->use();

				// TODO: do not upadte each frame !
				const Object3D::Camera & cam = VTXApp::get().getScene().getCamera();
				glUniformMatrix4fv( _uModelViewMatrix,
									1,
									GL_FALSE,
									Util::Math::value_ptr( cam.getViewMatrix() * _getModel().getTransform().get() ) );
				glUniformMatrix4fv( _uProjMatrix, 1, GL_FALSE, Util::Math::value_ptr( cam.getProjectionMatrix() ) );
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
