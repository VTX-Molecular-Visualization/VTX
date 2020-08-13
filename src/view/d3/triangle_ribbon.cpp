#include "triangle_ribbon.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			void TriangleRibbon::createProgram()
			{
				Renderer::GLSL::ProgramManager & pm = VTXApp::get().getProgramManager();
				_program = pm.createProgram( "Triangle", { "triangle.vert", "triangle.frag" } );
			}

			void TriangleRibbon::setUniFormLocations()
			{
				assert( _program != nullptr );
				_uModelViewMatrixLoc = glGetUniformLocation( _program->getId(), "uMVMatrix" );
				_uProjMatrixLoc		 = glGetUniformLocation( _program->getId(), "uProjMatrix" );
				_uNormalMatrixLoc	 = glGetUniformLocation( _program->getId(), "uNormalMatrix" );
			}

			void TriangleRibbon::render()
			{
				_program->use();

				// TODO: do not upadte each frame !
				const Object3D::Camera & cam	  = VTXApp::get().getScene().getCamera();
				const Mat4f				 MVMatrix = cam.getViewMatrix() * _model->getTransform().get();
				glUniformMatrix4fv( _uModelViewMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( MVMatrix ) );
				glUniformMatrix4fv( _uProjMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( cam.getProjectionMatrix() ) );
				glUniformMatrix4fv( _uNormalMatrixLoc,
									1,
									GL_FALSE,
									Util::Math::value_ptr( Util::Math::transpose( Util::Math::inverse( MVMatrix ) ) ) );

				for ( const std::pair<uint, uint> & pair :
					  _model->getMolecule()->getRepresentationState()[ Generic::REPRESENTATION::CARTOON ].triangles )
				{
					glDrawElements(
						GL_TRIANGLES, pair.second, GL_UNSIGNED_INT, (void *)( pair.first * sizeof( uint ) ) );
				}
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
