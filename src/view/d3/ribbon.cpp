#include "ribbon.hpp"
#include "representation/representation_target.hpp"
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

				_gl()->glPatchParameteri( GL_PATCH_VERTICES, 4 );
				_gl()->glGetIntegerv( GL_MAX_PATCH_VERTICES, &maxPatchVertices );
				_gl()->glGetIntegerv( GL_MAX_TESS_GEN_LEVEL, &maxTessGenLevel );

				VTX_DEBUG( "Max supported patch vertices: " + std::to_string( maxPatchVertices ) );
				VTX_DEBUG( "Max supported tessellation levels: " + std::to_string( maxTessGenLevel ) );

				Renderer::GLSL::ProgramManager & pm = VTXApp::get().getProgramManager();
				_program							= pm.createProgram( "Ribbon", { "ribbon_patch.vert", "ribbon_patch.tesc", "ribbon_patch.tese", "ribbon_patch.frag" } );

				assert( _program != nullptr );
				_uCamPositionLoc	 = _gl()->glGetUniformLocation( _program->getId(), "u_camPosition" );
				_uModelViewMatrixLoc = _gl()->glGetUniformLocation( _program->getId(), "u_MVMatrix" );
				_uProjMatrixLoc		 = _gl()->glGetUniformLocation( _program->getId(), "u_projMatrix" );
				_uNormalMatrixLoc	 = _gl()->glGetUniformLocation( _program->getId(), "u_normalMatrix" );
				_uMaxIndice			 = _gl()->glGetUniformLocation( _program->getId(), "u_maxIndice" );
			}

			void Ribbon::render()
			{
				_program->use();

				// TODO: do not upadte each frame !
				const Object3D::Camera & cam	  = VTXApp::get().getScene().getCamera();
				const Mat4f				 MVMatrix = cam.getViewMatrix() * _model->getMolecule()->getTransform().get();
				_gl()->glUniform3fv( _uCamPositionLoc, 1, (const GLfloat *)Util::Math::value_ptr( cam.getPosition() ) );
				_gl()->glUniformMatrix4fv( _uModelViewMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( MVMatrix ) );
				_gl()->glUniformMatrix4fv( _uProjMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( cam.getProjectionMatrix() ) );
				_gl()->glUniformMatrix4fv( _uNormalMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( Util::Math::transpose( Util::Math::inverse( MVMatrix ) ) ) );

				for ( const std::pair<const Model::Representation::InstantiatedRepresentation *, VTX::Representation::RepresentationTarget> representationData :
					  _model->getMolecule()->getRepresentationData() )
				{
					if ( !representationData.first->hasToDrawRibbon() )
						continue;

					for ( const std::pair<uint, uint> & ribbonData : representationData.second.getRibbons() )
					{
						_gl()->glUniform1ui( _uMaxIndice, ribbonData.second / 2u );
						_gl()->glDrawElements( GL_PATCHES, ribbonData.second, GL_UNSIGNED_INT, (void *)( ribbonData.first * sizeof( uint ) ) );
					}
				}

				// glDrawElements( GL_PATCHES, uint( _model->getIndices().size() ), GL_UNSIGNED_INT, 0 );
			}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
