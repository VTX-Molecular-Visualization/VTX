#include "sphere.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			Sphere::Sphere( Model::Molecule * const p_model ) : BaseView3DMolecule( p_model )
			{
				Renderer::GLSL::ProgramManager & pm = VTXApp::get().getProgramManager();
				_program							= pm.createProgram( "Sphere", { "sphere.vert", "sphere.geom", "sphere.frag" } );

				assert( _program != nullptr );
				_uModelViewMatrixLoc = OGL().glGetUniformLocation( _program->getId(), "uMVMatrix" );
				_uProjMatrixLoc		 = OGL().glGetUniformLocation( _program->getId(), "uProjMatrix" );
				_uRadiusFixedLoc	 = OGL().glGetUniformLocation( _program->getId(), "uRadiusFixed" );
				_uRadiusAddLoc		 = OGL().glGetUniformLocation( _program->getId(), "uRadiusAdd" );
				_uIsRadiusFixedLoc	 = OGL().glGetUniformLocation( _program->getId(), "uIsRadiusFixed" );
			}

			void Sphere::render( const Generic::REPRESENTATION p_representation )
			{
				switch ( p_representation )
				{
				case Generic::REPRESENTATION::BALL_AND_STICK:
					_radiusFixed   = VTX_SETTING().atomsRadius;
					_radiusAdd	   = 0.f;
					_isRadiusFixed = true;
					break;
				case Generic::REPRESENTATION::VAN_DER_WAALS:
					_isRadiusFixed = false;
					_radiusAdd	   = 0.f;
					break;
				case Generic::REPRESENTATION::STICK:
					_radiusFixed   = VTX_SETTING().bondsRadius;
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
				OGL().glUniformMatrix4fv( _uModelViewMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( cam.getViewMatrix() * _model->getTransform().get() ) );
				OGL().glUniformMatrix4fv( _uProjMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( cam.getProjectionMatrix() ) );

				OGL().glUniform1f( _uRadiusFixedLoc, _radiusFixed );
				OGL().glUniform1f( _uRadiusAddLoc, _radiusAdd );
				OGL().glUniform1ui( _uIsRadiusFixedLoc, _isRadiusFixed );

				for ( const std::pair<uint, uint> & pair : _model->getRepresentationState()[ p_representation ].atoms )
				{
					OGL().glDrawArrays( GL_POINTS, pair.first, pair.second );
				}
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
