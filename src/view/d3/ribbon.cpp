#include "ribbon.hpp"
#include "representation/representation_target.hpp"
#include "vtx_app.hpp"

namespace VTX::View::D3
{
	Renderer::GL::Program * const Ribbon::_createProgram()
	{
		return VTX_PROGRAM_MANAGER().createProgram(
			"Ribbon", { "ribbon_patch.vert", "ribbon_patch.tesc", "ribbon_patch.tese", "ribbon_patch.frag" } );
	}

	void Ribbon::_init()
	{
		_uCamPositionLoc = _gl()->glGetUniformLocation( _program->getId(), "u_camPosition" );
		_uMaxIndiceLoc	 = _gl()->glGetUniformLocation( _program->getId(), "u_maxIndice" );

		GLint maxPatchVertices = 0;
		GLint maxTessGenLevel  = 0;

		_gl()->glPatchParameteri( GL_PATCH_VERTICES, 4 );
		_gl()->glGetIntegerv( GL_MAX_PATCH_VERTICES, &maxPatchVertices );
		_gl()->glGetIntegerv( GL_MAX_TESS_GEN_LEVEL, &maxTessGenLevel );

		VTX_DEBUG( "Max supported patch vertices: " + std::to_string( maxPatchVertices ) );
		VTX_DEBUG( "Max supported tessellation levels: " + std::to_string( maxTessGenLevel ) );
	}

	void Ribbon::_render()
	{
		if ( VTXApp::get().MASK & VTX_MASK_CAMERA_UPDATED )
		{
			const Object3D::Camera & cam = VTXApp::get().getScene().getCamera();
			_gl()->glUniform3fv( _uCamPositionLoc, 1, (const GLfloat *)Util::Math::value_ptr( cam.getPosition() ) );
		}

		for ( const std::pair<const Model::Representation::InstantiatedRepresentation *,
							  VTX::Representation::RepresentationTarget> representationData :
			  _model->getMolecule()->getRepresentationData() )
		{
			if ( !representationData.first->hasToDrawRibbon() )
				continue;

			for ( const std::pair<uint, uint> & ribbonData : representationData.second.getRibbons() )
			{
				_gl()->glUniform1ui( _uMaxIndiceLoc, ribbonData.second / 2u );
				_gl()->glDrawElements(
					GL_PATCHES, ribbonData.second, GL_UNSIGNED_INT, (void *)( ribbonData.first * sizeof( uint ) ) );
			}
		}
	}
} // namespace VTX::View::D3
