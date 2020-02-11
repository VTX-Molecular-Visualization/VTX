#include "cylinder.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			void Cylinder::initItem()
			{
				Renderer::GLSLProgramManager & pm	   = VTXApp::get().getProgramManager();
				Renderer::GLSLProgram *		   program = pm.createProgram( "CylinderGeom" );
				program->attachShader( pm.createShader( "cylinderImpostorGeom.vert" ) );
				program->attachShader( pm.createShader( "cylinderImpostorGeom.geom" ) );
				program->attachShader( pm.createShader( "cylinderImpostorDeferred.frag" ) );
				program->link();

				_uViewModelMatrix = glGetUniformLocation( program->getId(), "uMVMatrix" );
				_uProjMatrix	  = glGetUniformLocation( program->getId(), "uProjMatrix" );
				_uRadius		  = glGetUniformLocation( program->getId(), "uCylRad" );
			}

			void Cylinder::notify( Event::EVENT_MODEL p_event )
			{
				BaseView3DMolecule::notify( p_event );

				if ( p_event == Event::EVENT_MODEL::CHANGE_REPRESENTATION )
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
