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
				Renderer::GLSL::ProgramManager & pm		 = VTXApp::get().getProgramManager();
				Renderer::GLSL::Program *		 program = pm.createProgram(
					   "CylinderGeom",
					   { "cylinderImpostorGeom.vert", "cylinderImpostorGeom.geom", "cylinderImpostorDeferred.frag" } );
				_uViewModelMatrix = glGetUniformLocation( program->getId(), "uMVMatrix" );
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

				for ( const std::pair<uint, uint> & pair :
					  _getModel().getRepresentationState()[ p_representation ].bonds )
				{
					VTXApp::get().getProgramManager().getProgram( "CylinderGeom" )->use();
					_setCameraUniforms( VTXApp::get().getScene().getCamera() );
					glUniform1f( _uRadius, 0.15f );

					glDrawRangeElements(
						GL_LINES, pair.first, pair.first + pair.second, pair.second * 2, GL_UNSIGNED_INT, NULL );
				}
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
