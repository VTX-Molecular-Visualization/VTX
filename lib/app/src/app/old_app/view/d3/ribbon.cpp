#include "app/old_app/view/d3/ribbon.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/render/camera.hpp"
#include "app/application/scene.hpp"
#include "app/application/representation/representation_target.hpp"
#include "app/old_app/vtx_app.hpp"

namespace VTX::View::D3
{
	Renderer::GL::Program * const Ribbon::_createProgram()
	{
		return VTX_PROGRAM_MANAGER().createProgram( "Ribbon",
													{ FilePath( "ribbon/ribbon_patch.vert" ),
													  FilePath( "ribbon/ribbon_patch.tesc" ),
													  FilePath( "ribbon/ribbon_patch.tese" ),
													  FilePath( "ribbon/ribbon_patch.frag" ) } );
	}

	// TODO reimplement this without Qt (OpenGL embedded in Qt)
	void Ribbon::_init()
	{
		//_gl->glPatchParameteri( GL_PATCH_VERTICES, 4 );
	}

	void Ribbon::render( const App::Component::Render::Camera & p_camera ) const
	{
		BaseView3D::render( p_camera );

		if ( VTXApp::get().MASK & VTX_MASK_CAMERA_UPDATED )
		{
			const App::Component::Render::Camera & cam = VTXApp::get().getScene().getCamera();
			_program->setVec3f( "u_camPosition", cam.getPosition() );
		}

		for ( const std::pair<const App::Application::Representation::InstantiatedRepresentation * const,
							  App::Application::Representation::RepresentationTarget> & representationData :
			  _model->getMolecule()->getRepresentationData() )
		{
			if ( !representationData.first->hasToDrawRibbon() )
			{
				continue;
			}

			_program->setUInt( "u_colorBlendingMode",
							   uint( representationData.first->getRibbonData().colorBlendingMode ) );

			const App::Application::Representation::TargetRange<void *> & target = representationData.second.getRibbons();
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
