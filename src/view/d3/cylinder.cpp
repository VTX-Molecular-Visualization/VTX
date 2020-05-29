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

			void Cylinder::render( const Generic::REPRESENTATION p_representation,
								   const std::map<uint, uint> &	 p_mapIdx )
			{
				return;

				switch ( p_representation )
				{
				case Generic::REPRESENTATION::STICK:
				case Generic::REPRESENTATION::BALL_AND_STICK: break;
				case Generic::REPRESENTATION::VAN_DER_WAALS:
				case Generic::REPRESENTATION::SAS:
				default: return;
				}

				for ( const std::pair<uint, uint> & pair : p_mapIdx )
				{
					VTXApp::get().getProgramManager().getProgram( "CylinderGeom" )->use();
					_setCameraUniforms( VTXApp::get().getScene().getCamera() );
					glUniform1f( _uRadius, 0.15f );
					glDrawRangeElements( GL_LINES,
										 0,
										 _getModel().getBondCount(),
										 _getModel().getBondCount() * 2,
										 GL_UNSIGNED_INT,
										 NULL );
				}
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
