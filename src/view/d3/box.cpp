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

				_uViewModelMatrix = glGetUniformLocation( program->getId(), "uMVMatrix" );
				_uProjMatrix	  = glGetUniformLocation( program->getId(), "uProjMatrix" );

				_isActive = true;
			}

			void Box::notify( const Event::VTX_EVENT_MODEL & p_event )
			{
				BaseView3DMolecule::notify( p_event );

				if ( p_event == Event::VTX_EVENT_MODEL::CHANGE_REPRESENTATION ) {}
			};

			void Box::render()
			{
				VTXApp::get().getProgramManager().getProgram( "LineShader" )->use();
				_setCameraUniforms( VTXApp::get().getScene().getCamera() );
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
