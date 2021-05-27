#include "cylinder.hpp"

namespace VTX::View::D3
{
	Renderer::GL::Program * const Cylinder::_createProgram()
	{
		return VTX_PROGRAM_MANAGER().createProgram( "Cylinder", { "cylinder.vert", "cylinder.geom", "cylinder.frag" } );
	}

	void Cylinder::_init() {}

	void Cylinder::render( const Object3D::Camera & p_camera ) const
	{
		BaseView3D::render( p_camera );

		for ( const std::pair<const Model::Representation::InstantiatedRepresentation *,
							  VTX::Representation::RepresentationTarget> & representationData :
			  _model->getMolecule()->getRepresentationData() )
		{
			if ( representationData.first->hasToDrawCylinder() )
			{
				const Model::Representation::CylinderData & cylinderData = representationData.first->getCylinderData();

				/// TODO: put a mask
				_program->setFloat( "u_cylRad", cylinderData._radius );

				const Representation::TargetRange & data = representationData.second.getBonds();
				/// TODO: use glDrawRangeElements?
				_model->getBuffer()->getVao().drawElement( Renderer::GL::VertexArray::DrawMode::LINES,
														   data.second,
														   Renderer::GL::VertexArray::Type::UNSIGNED_INT,
														   (void *)( data.first * sizeof( uint ) ) );
			}
		}
	}
} // namespace VTX::View::D3
