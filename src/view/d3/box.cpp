#include "box.hpp"
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
				_setCameraUniforms( VTXApp::get().getScene().getCamera() );
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
