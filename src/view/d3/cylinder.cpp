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
				_uModelViewMatrixLoc = glGetUniformLocation( _program->getId(), "uMVMatrix" );
				_uProjMatrixLoc		 = glGetUniformLocation( _program->getId(), "uProjMatrix" );
				_uRadiusLoc			 = glGetUniformLocation( _program->getId(), "uCylRad" );
			}

			void Cylinder::render( const Generic::REPRESENTATION p_representation )
			{
				switch ( p_representation )
				{
				case Generic::REPRESENTATION::STICK:
				case Generic::REPRESENTATION::BALL_AND_STICK: break;
				default: return;
				}

				_program->use();

				// TODO: do not upadte each frame !
				const Object3D::Camera & cam = VTXApp::get().getScene().getCamera();
				glUniformMatrix4fv( _uModelViewMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( cam.getViewMatrix() * _model->getTransform().get() ) );
				glUniformMatrix4fv( _uProjMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( cam.getProjectionMatrix() ) );
				glUniform1f( _uRadiusLoc, VTX_SETTING().bondsRadius );

				for ( const std::pair<uint, uint> & pair : _model->getRepresentationState()[ p_representation ].bonds )
				{
					glDrawElements( GL_LINES, pair.second, GL_UNSIGNED_INT, (void *)( pair.first * sizeof( uint ) ) );
				}
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
