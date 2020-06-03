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

			void Sphere::render( const Generic::REPRESENTATION p_representation )
			{
				switch ( p_representation )
				{
				case Generic::REPRESENTATION::BALL_AND_STICK:
					_radiusFixed   = 0.4f;
					_radiusAdd	   = 0.f;
					_isRadiusFixed = true;
					break;
				case Generic::REPRESENTATION::VAN_DER_WAALS:
					_isRadiusFixed = false;
					_radiusAdd	   = 0.f;
					break;
				case Generic::REPRESENTATION::STICK:
					_radiusFixed   = 0.15f;
					_radiusAdd	   = 0.f;
					_isRadiusFixed = true;
					break;
				case Generic::REPRESENTATION::SAS:
					_isRadiusFixed = false;
					_radiusAdd	   = 1.4f;
					break;
				default: return;
				}

				VTXApp::get().getProgramManager().getProgram( "SphereImpostorGeomShader" )->use();
				glUniform1f( _uRadiusFixedLoc, _radiusFixed );
				glUniform1f( _uRadiusAddLoc, _radiusAdd );
				glUniform1ui( _uIsRadiusFixedLoc, _isRadiusFixed );
				
				_setCameraUniforms( VTXApp::get().getScene().getCamera() );

				for ( const std::pair<uint, uint> & pair :
					  _getModel().getRepresentationState()[ p_representation ].atoms )
				{
					glDrawArrays( GL_POINTS, pair.first, pair.second );
				}
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
