#include "app/render/view/sphere.hpp"
#include "app/application/representation/representation_manager.hpp"
#include "app/old_app/vtx_app.hpp"
#include <util/logger.hpp>

namespace VTX::App::Render::View
{
	Renderer::GL::Program * const Sphere::_createProgram()
	{
		return VTX_PROGRAM_MANAGER().createProgram(
			"Sphere",
			{ FilePath( "sphere/sphere.vert" ), FilePath( "sphere/sphere.geom" ), FilePath( "sphere/sphere.frag" ) } );
	}

	void Sphere::_init() {}

	void Sphere::render( const App::Component::Render::Camera & p_camera ) const
	{
		BaseView3D::render( p_camera );

		for ( const std::pair<const App::Application::Representation::InstantiatedRepresentation * const,
							  App::Application::Representation::RepresentationTarget> & representationData :
			  _model->getMolecule()->getRepresentationData() )
		{
			if ( representationData.first->hasToDrawSphere() )
			{
				const App::Application::Representation::Primitive::Sphere & sphereData
					= representationData.first->getSphereData();

				/// TODO: put a mask
				_program->setFloat( "u_radiusFixed", sphereData.radiusFixed );
				_program->setFloat( "u_radiusAdd", sphereData.radiusAdd );
				_program->setBool( "u_isRadiusFixed", sphereData.isRadiusFixed );
				_program->setBool( "u_isPerspective", p_camera.isPerspective() );

				const App::Application::Representation::TargetRange<uint> & target
					= representationData.second.getAtoms();
				if ( target.counts.size() > 0 )
				{
					_model->getBuffer()->getVao().multiDrawArray( Renderer::GL::VertexArray::DrawMode::POINTS,
																  (GLint *)( &target.indices[ 0 ] ),
																  (GLsizei *)( &target.counts[ 0 ] ),
																  GLsizei( target.indices.size() ) );
				}
			}
		}
	}
} // namespace VTX::App::Render::View