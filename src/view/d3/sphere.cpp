#include "sphere.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			void Sphere::createProgram()
			{
				Renderer::GLSL::ProgramManager & pm = VTXApp::get().getProgramManager();
				_program = pm.createProgram( "Sphere", { "sphere.vert", "sphere.geom", "sphere.frag" } );
			}

			void Sphere::setUniFormLocations()
			{
				assert( _program != nullptr );
				_uModelViewMatrixLoc = glGetUniformLocation( _program->getId(), "uMVMatrix" );
				_uProjMatrixLoc		 = glGetUniformLocation( _program->getId(), "uProjMatrix" );
				_uRadiusFixedLoc	 = glGetUniformLocation( _program->getId(), "uRadiusFixed" );
				_uRadiusAddLoc		 = glGetUniformLocation( _program->getId(), "uRadiusAdd" );
				_uIsRadiusFixedLoc	 = glGetUniformLocation( _program->getId(), "uIsRadiusFixed" );
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

				_program->use();

				// TODO: do not upadte each frame !
				const Object3D::Camera & cam = VTXApp::get().getScene().getCamera();
				glUniformMatrix4fv( _uModelViewMatrixLoc,
									1,
									GL_FALSE,
									Util::Math::value_ptr( cam.getViewMatrix() * _getModel().getTransform().get() ) );
				glUniformMatrix4fv( _uProjMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( cam.getProjectionMatrix() ) );

				glUniform1f( _uRadiusFixedLoc, _radiusFixed );
				glUniform1f( _uRadiusAddLoc, _radiusAdd );
				glUniform1ui( _uIsRadiusFixedLoc, _isRadiusFixed );

				for ( const std::pair<uint, uint> & pair :
					  _getModel().getRepresentationState()[ p_representation ].atoms )
				{
					glDrawArrays( GL_POINTS, pair.first, pair.second );
				}
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
