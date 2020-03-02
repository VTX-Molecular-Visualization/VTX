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

			void Cylinder::notify( const Event::VTX_EVENT_MODEL & p_event )
			{
				BaseView3DMolecule::notify( p_event );

				if ( p_event == Event::VTX_EVENT_MODEL::CHANGE_REPRESENTATION )
				{
					switch ( Setting::Rendering::representation )
					{
					case MOLECULE_REPRESENTATION::STICK:
					case MOLECULE_REPRESENTATION::BALL_AND_STICK: _isActive = true; break;
					case MOLECULE_REPRESENTATION::VAN_DER_WAALS:
					default: _isActive = false; break;
					}
				}
			};

			void Cylinder::render()
			{
				VTXApp::get().getProgramManager().getProgram( "CylinderGeom" )->use();
				_setCameraUniforms( VTXApp::get().getScene().getCamera() );
				glUniform1f( _uRadius, 0.2f );
				glDrawElements( GL_LINES, _getModel().getBondCount(), GL_UNSIGNED_INT, (void *)( 0 * sizeof( uint ) ) );
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
