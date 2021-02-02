#include "cylinder.hpp"

namespace VTX::View::D3
{
	Renderer::GL::Program * const Cylinder::_createProgram()
	{
		return VTX_PROGRAM_MANAGER().createProgram( "Cylinder", { "cylinder.vert", "cylinder.geom", "cylinder.frag" } );
	}

	void Cylinder::_init() { _uRadiusLoc = _gl()->glGetUniformLocation( _program->getId(), "u_cylRad" ); }

	void Cylinder::render( const Object3D::Camera & p_camera )
	{
		BaseView3D::render( p_camera );

		for ( const std::pair<const Model::Representation::InstantiatedRepresentation *,
							  VTX::Representation::RepresentationTarget> representationData :
			  _model->getMolecule()->getRepresentationData() )
		{
			if ( !representationData.first->hasToDrawCylinder() )
				return;

			const Model::Representation::CylinderData & cylinderData = representationData.first->getCylinderData();
			_gl()->glUniform1f( _uRadiusLoc, cylinderData._radius );

			for ( const std::pair<uint, uint> & pair : representationData.second.getBonds() )
			{
				_gl()->glDrawElements(
					GL_LINES, pair.second, GL_UNSIGNED_INT, (void *)( pair.first * sizeof( uint ) ) );
			}
		}
	}
} // namespace VTX::View::D3
