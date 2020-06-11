#include "triangle_ribbon.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			void TriangleRibbon::init()
			{
				Renderer::GLSL::ProgramManager & pm = VTXApp::get().getProgramManager();
				Renderer::GLSL::Program *		 program
					= pm.createProgram( "Triangle", { "triangle.vert", "triangle.frag" } );

				_uModelViewMatrix = glGetUniformLocation( program->getId(), "uMVMatrix" );
				_uProjMatrix	  = glGetUniformLocation( program->getId(), "uProjMatrix" );
				_uNormalMatrix	  = glGetUniformLocation( program->getId(), "uNormalMatrix" );
			}

			void TriangleRibbon::render()
			{
				VTXApp::get().getProgramManager().getProgram( "Triangle" )->use();
				_setCameraUniforms( VTXApp::get().getScene().getCamera() );

				// TODO: MV is already computed in _setCameraUniforms !
				const Mat4f MVMatrix
					= VTXApp::get().getScene().getCamera().getViewMatrix() * _getModel().getTransform().get();

				glUniformMatrix4fv( _uNormalMatrix,
									1,
									GL_FALSE,
									Util::Math::value_ptr( Util::Math::transpose( Util::Math::inverse( MVMatrix ) ) ) );

				for ( const std::pair<uint, uint> & pair :
					  _getModel()
						  .getMolecule()
						  ->getRepresentationState()[ Generic::REPRESENTATION::CARTOON ]
						  .triangles )
				{
					glDrawElements(
						GL_TRIANGLES, pair.second, GL_UNSIGNED_INT, (void *)( pair.first * sizeof( uint ) ) );
				}
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
