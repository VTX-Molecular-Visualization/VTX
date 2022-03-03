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
		return VTX_PROGRAM_MANAGER().createProgram( "Ribbon",
													{ IO::FilePath( "ribbon_patch.vert" ),
													  IO::FilePath( "ribbon_patch.tesc" ),
													  IO::FilePath( "ribbon_patch.tese" ),
													  IO::FilePath( "ribbon_patch.frag" ) } );
	}

	void Ribbon::_init() { _gl()->glPatchParameteri( GL_PATCH_VERTICES, 4 ); }

	void Ribbon::render( const Object3D::Camera & p_camera ) const
	{
		BaseView3D::render( p_camera );

		if ( VTXApp::get().MASK & VTX_MASK_CAMERA_UPDATED )
		{
			const Object3D::Camera & cam = VTXApp::get().getScene().getCamera();
			_program->setVec3f( "u_camPosition", cam.getPosition() );
		}

		for ( const std::pair<const Model::Representation::InstantiatedRepresentation * const,
							  VTX::Representation::RepresentationTarget> & representationData :
			  _model->getMolecule()->getRepresentationData() )
		{
			if ( !representationData.first->hasToDrawRibbon() )
			{
				continue;
			}

			_program->setUInt( "u_transitionColorMode",
							   uint( representationData.first->getRibbonData()._colorBlendingMode ) );

			const Representation::TargetRange<void *> & target = representationData.second.getRibbons();
			if ( target.indices.size() > 0 )
			{
				_model->getBuffer()->getVao().multiDrawElement( Renderer::GL::VertexArray::DrawMode::PATCHES,
																(GLsizei *)( &target.counts[ 0 ] ),
																Renderer::GL::VertexArray::Type::UNSIGNED_INT,
																(GLvoid **)( &target.indices[ 0 ] ),
																GLsizei( target.indices.size() ) );
			}
		}
	}
} // namespace VTX::View::D3
