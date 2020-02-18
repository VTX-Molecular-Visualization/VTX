#include "sphere.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			void Sphere::init()
			{
				Renderer::GLSL::ProgramManager & pm		 = VTXApp::get().getProgramManager();
				Renderer::GLSL::Program *		 program = pm.createProgram( "SphereImpostorGeomShader" );
				program->attachShader( pm.createShader( "sphereImpostorGeom.vert" ) );
				program->attachShader( pm.createShader( "sphereImpostorGeomQuad.geom" ) );
				program->attachShader( pm.createShader( "sphereImpostorDeferred.frag" ) );
				program->link();

				_uViewModelMatrix = glGetUniformLocation( program->getId(), "uMVMatrix" );
				_uProjMatrix	  = glGetUniformLocation( program->getId(), "uProjMatrix" );
				_uRadiusScale	  = glGetUniformLocation( program->getId(), "uRadScale" );
			}

			void Sphere::notify( const Event::VTX_EVENT_MODEL & p_event )
			{
				BaseView3DMolecule::notify( p_event );

				if ( p_event == Event::VTX_EVENT_MODEL::CHANGE_REPRESENTATION )
				{
					switch ( Setting::Rendering::representation )
					{
					case MOLECULE_REPRESENTATION::BALL_AND_STICK:
						_radiusScale = 0.3f;
						_isActive	 = true;
						break;
					case MOLECULE_REPRESENTATION::VAN_DER_WAALS:
						_radiusScale = 1.0f;
						_isActive	 = true;
						break;
					case MOLECULE_REPRESENTATION::STICK:
						//_radiusScale = 0.1f;
						_isActive = false;
						break;
					default: _isActive = false; break;
					}
				}
			};

			void Sphere::render()
			{
				VTXApp::get().getProgramManager().getProgram( "SphereImpostorGeomShader" )->use();
				_setCameraUniforms( VTXApp::get().getScene().getCamera() );
				glUniform1f( _uRadiusScale, _radiusScale );
				glDrawArrays( GL_POINTS, 0, _getModel().getAtomCount() );
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
