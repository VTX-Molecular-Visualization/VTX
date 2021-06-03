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

				const Representation::TargetRange<void *> & target = representationData.second.getBonds();
				/// TODO: use glDrawRangeElements?
				assert( target.counts.size() > 0 );
				_model->getBuffer()->getVao().multiDrawElement( Renderer::GL::VertexArray::DrawMode::LINES,
																(GLsizei *)( &target.counts[ 0 ] ),
																Renderer::GL::VertexArray::Type::UNSIGNED_INT,
																(GLvoid **)( &target.indices[ 0 ] ),
																GLsizei( target.indices.size() ) );

				/*
				_model->getBuffer()->getVao().drawElement( Renderer::GL::VertexArray::DrawMode::LINES,
														   data.second,
														   Renderer::GL::VertexArray::Type::UNSIGNED_INT,
														   (void *)( data.first * sizeof( uint ) ) );
														   */
			}
		}
	}
} // namespace VTX::View::D3
