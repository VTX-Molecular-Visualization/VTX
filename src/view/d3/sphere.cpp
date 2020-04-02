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

				_uViewModelMatrix = glGetUniformLocation( program->getId(), "uMVMatrix" );
				_uProjMatrix	  = glGetUniformLocation( program->getId(), "uProjMatrix" );
				_uRadiusFixed	  = glGetUniformLocation( program->getId(), "uRadiusFixed" );
				_uIsRadiusFixed	  = glGetUniformLocation( program->getId(), "uIsRadiusFixed" );
			}

			void Sphere::notify( const Event::VTX_EVENT_MODEL & p_event )
			{
				BaseView3DMolecule::notify( p_event );

				if ( p_event == Event::VTX_EVENT_MODEL::CHANGE_REPRESENTATION )
				{
					switch ( Setting::Rendering::representation )
					{
					case MOLECULE_REPRESENTATION::BALL_AND_STICK:
						_radiusFixed = 0.4f;
						_isRadiusFixed = true;
						_isActive	 = true;
						break;
					case MOLECULE_REPRESENTATION::VAN_DER_WAALS:
						_isRadiusFixed = false;
						_isActive	 = true;
						break;
					case MOLECULE_REPRESENTATION::STICK:
						_radiusFixed   = 0.15f;
						_isRadiusFixed = true;
						_isActive	   = true;
						break;
					default: _isActive = false; break;
					}
				}
			};

			void Sphere::render()
			{
				VTXApp::get().getProgramManager().getProgram( "SphereImpostorGeomShader" )->use();
				_setCameraUniforms( VTXApp::get().getScene().getCamera() );
				glUniform1f( _uRadiusFixed, _radiusFixed );
				glUniform1ui( _uIsRadiusFixed, _isRadiusFixed );
				glDrawArrays( GL_POINTS, 0, _getModel().getAtomCount() );
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
