#include "sphere.hpp"
#include "representation/representation_manager.hpp"
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
				_uModelViewMatrixLoc = _gl()->glGetUniformLocation( _program->getId(), "uMVMatrix" );
				_uProjMatrixLoc		 = _gl()->glGetUniformLocation( _program->getId(), "uProjMatrix" );
				_uRadiusFixedLoc	 = _gl()->glGetUniformLocation( _program->getId(), "uRadiusFixed" );
				_uRadiusAddLoc		 = _gl()->glGetUniformLocation( _program->getId(), "uRadiusAdd" );
				_uIsRadiusFixedLoc	 = _gl()->glGetUniformLocation( _program->getId(), "uIsRadiusFixed" );
			}

			void Sphere::render( const Model::Representation::BaseRepresentation * const p_representation )
			{
				if ( !p_representation->hasToDrawSphere() )
					return;

				const Model::Representation::SphereData sphereData = p_representation->getSphereData();
				_isRadiusFixed									   = sphereData._isRadiusFixed;
				_radiusFixed									   = sphereData._radiusFixed;
				_radiusAdd										   = sphereData._radiusAdd;

				_program->use();

				// TODO: do not upadte each frame !
				const Object3D::Camera & cam = VTXApp::get().getScene().getCamera();
				_gl()->glUniformMatrix4fv( _uModelViewMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( cam.getViewMatrix() * _model->getTransform().get() ) );
				_gl()->glUniformMatrix4fv( _uProjMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( cam.getProjectionMatrix() ) );

				_gl()->glUniform1f( _uRadiusFixedLoc, _radiusFixed );
				_gl()->glUniform1f( _uRadiusAddLoc, _radiusAdd );
				_gl()->glUniform1ui( _uIsRadiusFixedLoc, _isRadiusFixed );

				for ( const std::pair<uint, uint> & pair : _model->getRepresentationAtoms( p_representation ) )
				{
					_gl()->glDrawArrays( GL_POINTS, pair.first, pair.second );
				}
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
