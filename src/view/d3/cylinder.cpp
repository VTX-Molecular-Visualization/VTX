#include "cylinder.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			void Cylinder::init()
			{
				Renderer::GLSL::ProgramManager & pm = VTXApp::get().getProgramManager();
				Renderer::GLSL::Program *		 program
					= pm.createProgram( "Cylinder", { "cylinder.vert", "cylinder.geom", "cylinder.frag" } );
				_uModelViewMatrix = glGetUniformLocation( program->getId(), "uMVMatrix" );
				_uProjMatrix	  = glGetUniformLocation( program->getId(), "uProjMatrix" );
				_uRadius		  = glGetUniformLocation( program->getId(), "uCylRad" );
			}

			void Cylinder::render( const Generic::REPRESENTATION p_representation )
			{
				switch ( p_representation )
				{
				case Generic::REPRESENTATION::STICK:
				case Generic::REPRESENTATION::BALL_AND_STICK: break;
				default: return;
				}

				VTXApp::get().getProgramManager().getProgram( "Cylinder" )->use();
				glUniform1f( _uRadius, 0.15f );
				_setCameraUniforms( VTXApp::get().getScene().getCamera() );

				for ( const std::pair<uint, uint> & pair :
					  _getModel().getRepresentationState()[ p_representation ].bonds )
				{
					glDrawElements( GL_LINES, pair.second, GL_UNSIGNED_INT, (void *)( pair.first * sizeof( uint ) ) );
				}
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
