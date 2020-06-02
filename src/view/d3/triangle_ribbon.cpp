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
					= pm.createProgram( "TriangleGeomShader", { "triangle.vert", "triangle.frag" } );

				_uViewModelMatrix = glGetUniformLocation( program->getId(), "uMVMatrix" );
				_uProjMatrix	  = glGetUniformLocation( program->getId(), "uProjMatrix" );
			}

			void TriangleRibbon::render()
			{
				VTXApp::get().getProgramManager().getProgram( "TriangleGeomShader" )->use();
				_setCameraUniforms( VTXApp::get().getScene().getCamera() );

				for ( const std::pair<uint, uint> & pair :
					  _getModel()
						  .getMolecule()
						  ->getRepresentationState()[ Generic::REPRESENTATION::CARTOON ]
						  .triangles )
				{
					glDrawRangeElements(
						GL_TRIANGLES, pair.first, pair.first + pair.second, pair.second, GL_UNSIGNED_INT, NULL );
				}
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
