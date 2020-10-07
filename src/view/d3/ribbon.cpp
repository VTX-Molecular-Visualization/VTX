#include "ribbon.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			Ribbon::Ribbon( Model::SecondaryStructure * const p_model ) : BaseView3D( p_model )
			{
				GLint maxPatchVertices = 0;
				GLint maxTessGenLevel  = 0;

				OGL().glPatchParameteri( GL_PATCH_VERTICES, 4 );
				OGL().glGetIntegerv( GL_MAX_PATCH_VERTICES, &maxPatchVertices );
				OGL().glGetIntegerv( GL_MAX_TESS_GEN_LEVEL, &maxTessGenLevel );

				VTX_DEBUG( "Max supported patch vertices: " + std::to_string( maxPatchVertices ) );
				VTX_DEBUG( "Max supported tessellation levels: " + std::to_string( maxTessGenLevel ) );

				Renderer::GLSL::ProgramManager & pm = VTXApp::get().getProgramManager();
				_program							= pm.createProgram( "Ribbon", { "ribbon_patch.vert", "ribbon_patch.tesc", "ribbon_patch.tese", "ribbon_patch.frag" } );

				assert( _program != nullptr );
				_uCamPositionLoc	 = OGL().glGetUniformLocation( _program->getId(), "u_camPosition" );
				_uModelViewMatrixLoc = OGL().glGetUniformLocation( _program->getId(), "u_MVMatrix" );
				_uProjMatrixLoc		 = OGL().glGetUniformLocation( _program->getId(), "u_projMatrix" );
				_uNormalMatrixLoc	 = OGL().glGetUniformLocation( _program->getId(), "u_normalMatrix" );
				_uMaxIndice			 = OGL().glGetUniformLocation( _program->getId(), "u_maxIndice" );
			}

			void Ribbon::render()
			{
				_program->use();

				// TODO: do not upadte each frame !
				const Object3D::Camera & cam	  = VTXApp::get().getScene().getCamera();
				const Mat4f				 MVMatrix = cam.getViewMatrix() * _model->getMolecule()->getTransform().get();
				OGL().glUniform3fv( _uCamPositionLoc, 1, (const GLfloat *)Util::Math::value_ptr( cam.getPosition() ) );
				OGL().glUniformMatrix4fv( _uModelViewMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( MVMatrix ) );
				OGL().glUniformMatrix4fv( _uProjMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( cam.getProjectionMatrix() ) );
				OGL().glUniformMatrix4fv( _uNormalMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( Util::Math::transpose( Util::Math::inverse( MVMatrix ) ) ) );

				for ( const std::pair<uint, uint> & pair : _model->getMolecule()->getRepresentationState()[ Generic::REPRESENTATION::CARTOON ].ribbons )
				{
					OGL().glUniform1ui( _uMaxIndice, pair.second / 2u );
					OGL().glDrawElements( GL_PATCHES, pair.second, GL_UNSIGNED_INT, (void *)( pair.first * sizeof( uint ) ) );
				}

				// glDrawElements( GL_PATCHES, uint( _model->getIndices().size() ), GL_UNSIGNED_INT, 0 );
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
