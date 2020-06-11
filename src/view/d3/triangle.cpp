#include "triangle.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			void Triangle::init()
			{
				Renderer::GLSL::ProgramManager & pm = VTXApp::get().getProgramManager();
				Renderer::GLSL::Program *		 program
					= pm.createProgram( "Triangle", { "triangle.vert", "triangle.frag" } );

				_uModelViewMatrix = glGetUniformLocation( program->getId(), "uMVMatrix" );
				_uProjMatrix	  = glGetUniformLocation( program->getId(), "uProjMatrix" );
			}

			void Triangle::render()
			{
				VTXApp::get().getProgramManager().getProgram( "Triangle" )->use();
				_setCameraUniforms( VTXApp::get().getScene().getCamera() );
				glDrawElements( GL_TRIANGLES, uint( _getModel().getIndices().size() ), GL_UNSIGNED_INT, 0 );
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
