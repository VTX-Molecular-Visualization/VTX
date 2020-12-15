#include "cylinder.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			Cylinder::Cylinder( Model::Molecule * const p_model ) : BaseView3DMolecule( p_model )
			{
				Renderer::GLSL::ProgramManager & pm = VTXApp::get().getProgramManager();
				_program							= pm.createProgram( "Cylinder", { "cylinder.vert", "cylinder.geom", "cylinder.frag" } );

				assert( _program != nullptr );
				_uModelViewMatrixLoc = _gl()->glGetUniformLocation( _program->getId(), "uMVMatrix" );
				_uProjMatrixLoc		 = _gl()->glGetUniformLocation( _program->getId(), "uProjMatrix" );
				_uRadiusLoc			 = _gl()->glGetUniformLocation( _program->getId(), "uCylRad" );
			}

			void Cylinder::render( const Model::Representation::InstantiatedRepresentation * const p_representation )
			{
				if ( !p_representation->hasToDrawCylinder() )
					return;

				const float radius = p_representation->getCylinderData()._radius;

				_program->use();

				// TODO: do not upadte each frame !
				const Object3D::Camera & cam = VTXApp::get().getScene().getCamera();
				_gl()->glUniformMatrix4fv( _uModelViewMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( cam.getViewMatrix() * _model->getTransform().get() ) );
				_gl()->glUniformMatrix4fv( _uProjMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( cam.getProjectionMatrix() ) );
				_gl()->glUniform1f( _uRadiusLoc, radius );

				for ( const std::pair<uint, uint> & pair : _model->getRepresentationBonds( p_representation ) )
				{
					_gl()->glDrawElements( GL_LINES, pair.second, GL_UNSIGNED_INT, (void *)( pair.first * sizeof( uint ) ) );
				}
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
