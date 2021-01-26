#include "sphere.hpp"
#include "representation/representation_manager.hpp"
#include "vtx_app.hpp"

namespace VTX::View::D3
{
	Renderer::GL::Program * const Sphere::_createProgram()
	{
		return VTX_PROGRAM_MANAGER().createProgram( "Sphere", { "sphere.vert", "sphere.geom", "sphere.frag" } );
	}

	void Sphere::_init()
	{
		_uRadiusFixedLoc   = _gl()->glGetUniformLocation( _program->getId(), "u_radiusFixed" );
		_uRadiusAddLoc	   = _gl()->glGetUniformLocation( _program->getId(), "u_radiusAdd" );
		_uIsRadiusFixedLoc = _gl()->glGetUniformLocation( _program->getId(), "u_isRadiusFixed" );
	}

	void Sphere::_render( const Model::Representation::InstantiatedRepresentation * const p_representation )
	{
		if ( !p_representation->hasToDrawSphere() )
			return;

		const Model::Representation::SphereData & sphereData = p_representation->getSphereData();

		_gl()->glUniform1f( _uRadiusFixedLoc, sphereData._isRadiusFixed );
		_gl()->glUniform1f( _uRadiusAddLoc, sphereData._radiusAdd );
		_gl()->glUniform1ui( _uIsRadiusFixedLoc, sphereData._radiusFixed );

		for ( const std::pair<uint, uint> & pair : _model->getRepresentationAtoms( p_representation ) )
		{
			_gl()->glDrawArrays( GL_POINTS, pair.first, pair.second );
		}
	}
} // namespace VTX::View::D3
