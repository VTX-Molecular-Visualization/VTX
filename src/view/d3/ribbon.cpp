#include "ribbon.hpp"
#include "model/molecule.hpp"
#include "object3d/camera.hpp"
#include "object3d/scene.hpp"
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
		GLint maxPatchVertices = 0;
		GLint maxTessGenLevel  = 0;

		_gl()->glPatchParameteri( GL_PATCH_VERTICES, 4 );
		_gl()->glGetIntegerv( GL_MAX_PATCH_VERTICES, &maxPatchVertices );
		_gl()->glGetIntegerv( GL_MAX_TESS_GEN_LEVEL, &maxTessGenLevel );

		VTX_DEBUG( "Max supported patch vertices: " + std::to_string( maxPatchVertices ) );
		VTX_DEBUG( "Max supported tessellation levels: " + std::to_string( maxTessGenLevel ) );
	}

	void Ribbon::render( const Object3D::Camera & p_camera )
	{
		BaseView3D::render( p_camera );

		if ( VTXApp::get().MASK & VTX_MASK_CAMERA_UPDATED )
		{
			const Object3D::Camera & cam = VTXApp::get().getScene().getCamera();
			_program->setVec3f( "u_camPosition", cam.getPosition() );
		}
		/// TODO: remove
		//_gl()->glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		for ( const std::pair<const Model::Representation::InstantiatedRepresentation *,
							  VTX::Representation::RepresentationTarget> & representationData :
			  _model->getMolecule()->getRepresentationData() )
		{
			if ( !representationData.first->hasToDrawRibbon() )
				continue;

			for ( const std::pair<uint, uint> & ribbonData : representationData.second.getRibbons() )
			{
				_gl()->glDrawElements(
					GL_PATCHES, ribbonData.second, GL_UNSIGNED_INT, (void *)( ribbonData.first * sizeof( uint ) ) );
			}
		}
		//_gl()->glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}
} // namespace VTX::View::D3
