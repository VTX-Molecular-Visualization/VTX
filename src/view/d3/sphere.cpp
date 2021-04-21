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

	void Sphere::_init()
	{
		_uRadiusFixedLoc   = _program->getUniformLocation( "u_radiusFixed" );
		_uRadiusAddLoc	   = _program->getUniformLocation( "u_radiusAdd" );
		_uIsRadiusFixedLoc = _program->getUniformLocation( "u_isRadiusFixed" );
	}

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

			_gl()->glUniform1f( _uRadiusFixedLoc, sphereData._radiusFixed );
			_gl()->glUniform1f( _uRadiusAddLoc, sphereData._radiusAdd );
			_gl()->glUniform1ui( _uIsRadiusFixedLoc, sphereData._isRadiusFixed );

			for ( const std::pair<uint, uint> & pair : representationData.second.getAtoms() )
			{
				_gl()->glDrawArrays( GL_POINTS, pair.first, pair.second );
			}
		}
	}
} // namespace VTX::View::D3
