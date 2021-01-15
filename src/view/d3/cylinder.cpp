#include "cylinder.hpp"
#include "vtx_app.hpp"

namespace VTX::View::D3
{
	Renderer::GL::Program * const Cylinder::_createProgram( Renderer::GL::ProgramManager & p_pm )
	{
		return p_pm.createProgram( "Cylinder", { "cylinder.vert", "cylinder.geom", "cylinder.frag" } );
	}

	void Cylinder::_createUniforms() { _uRadiusLoc = _gl()->glGetUniformLocation( _program->getId(), "u_cylRad" ); }

	void Cylinder::_render( const Model::Representation::InstantiatedRepresentation * const p_representation )
	{
		if ( !p_representation->hasToDrawCylinder() )
			return;

		_gl()->glUniform1f( _uRadiusLoc, p_representation->getCylinderData()._radius );

		for ( const std::pair<uint, uint> & pair : _model->getRepresentationBonds( p_representation ) )
		{
			_gl()->glDrawElements( GL_LINES, pair.second, GL_UNSIGNED_INT, (void *)( pair.first * sizeof( uint ) ) );
		}
	}
} // namespace VTX::View::D3
