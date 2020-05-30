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
				_uRadiusAdd		  = glGetUniformLocation( program->getId(), "uRadiusAdd" );
				_uIsRadiusFixed	  = glGetUniformLocation( program->getId(), "uIsRadiusFixed" );
			}

			void Sphere::render( const Generic::REPRESENTATION p_representation,
								 const std::map<uint, uint> &  p_mapRangeAtoms,
								 const std::map<uint, uint> &  p_mapRangeBonds )
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
				_setCameraUniforms( VTXApp::get().getScene().getCamera() );
				glUniform1f( _uRadiusFixed, _radiusFixed );
				glUniform1f( _uRadiusAdd, _radiusAdd );
				glUniform1ui( _uIsRadiusFixed, _isRadiusFixed );

				for ( const std::pair<uint, uint> & pair : p_mapRangeAtoms )
				{
					glDrawArrays( GL_POINTS, pair.first, pair.second );
				}
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
