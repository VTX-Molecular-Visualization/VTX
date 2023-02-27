#include "sphere.hpp"
#include "representation/representation_manager.hpp"
#include "tool/logger.hpp"
#include "vtx_app.hpp"

namespace VTX::View::D3
{
	Renderer::GL::Program * const Sphere::_createProgram()
	{
		return VTX_PROGRAM_MANAGER().createProgram( "Sphere",
													{ IO::FilePath( "sphere/sphere.vert" ),
													  IO::FilePath( "sphere/sphere.geom" ),
													  IO::FilePath( "sphere/sphere.frag" ) } );
	}

	void Sphere::_init() {}

	void Sphere::render( const Object3D::Camera & p_camera ) const
	{
		BaseView3D::render( p_camera );

		for ( const std::pair<const Model::Representation::InstantiatedRepresentation * const,
							  VTX::Representation::RepresentationTarget> & representationData :
			  _model->getMolecule()->getRepresentationData() )
		{
			if ( representationData.first->hasToDrawSphere() )
			{
				const Model::Representation::SphereData & sphereData = representationData.first->getSphereData();

				/// TODO: put a mask
				_program->setFloat( "u_radiusFixed", sphereData.radiusFixed );
				_program->setFloat( "u_radiusAdd", sphereData.radiusAdd );
				_program->setBool( "u_isRadiusFixed", sphereData.isRadiusFixed );
				_program->setBool( "u_isPerspective", p_camera.isPerspective() );

				const Representation::TargetRange<uint> & target = representationData.second.getAtoms();
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
} // namespace VTX::View::D3
