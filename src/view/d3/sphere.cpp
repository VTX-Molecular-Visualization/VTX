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
				Renderer::GLSL::Program *		 program = pm.createProgram(
					   "SphereImpostorGeomShader",
					   { "sphereImpostorGeom.vert", "sphereImpostorGeomQuad.geom", "sphereImpostorDeferred.frag" } );

				_uViewModelMatrix  = glGetUniformLocation( program->getId(), "uMVMatrix" );
				_uProjMatrix	   = glGetUniformLocation( program->getId(), "uProjMatrix" );
				_uRadiusFixedLoc   = glGetUniformLocation( program->getId(), "uRadiusFixed" );
				_uRadiusAddLoc	   = glGetUniformLocation( program->getId(), "uRadiusAdd" );
				_uIsRadiusFixedLoc = glGetUniformLocation( program->getId(), "uIsRadiusFixed" );
			}

			void Sphere::notify( const Event::VTX_EVENT_MODEL & p_event )
			{
				BaseView3DMolecule::notify( p_event );

				if ( p_event == Event::VTX_EVENT_MODEL::CHANGE_REPRESENTATION )
				{
					switch ( Setting::Rendering::representation )
					{
					case MOLECULE_REPRESENTATION::BALL_AND_STICK:
						_radiusFixed   = 0.4f;
						_radiusAdd	   = 0.f;
						_isRadiusFixed = true;
						_isActive	   = true;
						break;
					case MOLECULE_REPRESENTATION::VAN_DER_WAALS:
						_isRadiusFixed = false;
						_radiusAdd	   = 0.f;
						_isActive	   = true;
						break;
					case MOLECULE_REPRESENTATION::STICK:
						_radiusFixed   = 0.15f;
						_radiusAdd	   = 0.f;
						_isRadiusFixed = true;
						_isActive	   = false;
						break;
					case MOLECULE_REPRESENTATION::SAS:
						_isRadiusFixed = false;
						_radiusAdd	   = 1.4f;
						_isActive	   = true;
						break;
					default: _isActive = false; break;
					}
				}
			};

			void Sphere::render()
			{
				VTXApp::get().getProgramManager().getProgram( "SphereImpostorGeomShader" )->use();
				// TODO: do not update each frame
				glUniform1f( _uRadiusFixedLoc, _radiusFixed );
				glUniform1f( _uRadiusAddLoc, _radiusAdd );
				glUniform1ui( _uIsRadiusFixedLoc, _isRadiusFixed );

				_setCameraUniforms( VTXApp::get().getScene().getCamera() );
				glDrawArrays( GL_POINTS, 0, _getModel().getAtomCount() );
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
