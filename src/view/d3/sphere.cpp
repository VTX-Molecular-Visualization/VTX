#include "sphere.hpp"
#include "representation/representation_manager.hpp"
#include "tool/logger.hpp"
#include "vtx_app.hpp"

namespace VTX::View::D3
{
	Renderer::GL::Program * const Sphere::_createProgram()
	{
		return VTX_PROGRAM_MANAGER().createProgram( "Sphere", { "sphere.vert", "sphere.geom", "sphere.frag" } );
	}

	void Sphere::_init() {}

	void Sphere::render( const Object3D::Camera & p_camera )
	{
		BaseView3D::render( p_camera );

		for ( const std::pair<const Model::Representation::InstantiatedRepresentation *,
							  VTX::Representation::RepresentationTarget> & representationData :
			  _model->getMolecule()->getRepresentationData() )
		{
			if ( !representationData.first->hasToDrawSphere() )
				continue;

			const Model::Representation::SphereData & sphereData = representationData.first->getSphereData();

			/// TODO: put a mask
			_program->setFloat( "u_radiusFixed", sphereData._radiusFixed );
			_program->setFloat( "u_radiusAdd", sphereData._radiusAdd );
			_program->setBool( "u_isRadiusFixed", sphereData._isRadiusFixed );

			for ( const std::pair<uint, uint> & pair : representationData.second.getAtoms() )
			{
				_gl()->glDrawArrays( GL_POINTS, pair.first, pair.second );
			}
		}
	}
} // namespace VTX::View::D3
