#include "triangle.hpp"
#include "model/category.hpp"
#include "model/molecule.hpp"
#include "representation/representation_manager.hpp"

namespace VTX::View::D3
{
	Renderer::GL::Program * const Triangle::_createProgram()
	{
		return VTX_PROGRAM_MANAGER().createProgram(
			"Triangle", { FilePath( "triangle/triangle.vert" ), FilePath( "triangle/triangle.frag" ) } );
	}

	void Triangle::render( const Object3D::Camera & p_camera ) const
	{
		BaseView3D::render( p_camera );

		_model->getBuffer()->getVao().drawElement( Renderer::GL::VertexArray::DrawMode::TRIANGLES,
												   GLsizei( _model->getIndices().size() ),
												   Renderer::GL::VertexArray::Type::UNSIGNED_INT );
	}

	Renderer::GL::Program * const TriangleSES::_createProgram()
	{
		return VTX_PROGRAM_MANAGER().createProgram(
			"Triangle", { FilePath( "triangle/triangle.vert" ), FilePath( "triangle/triangle.frag" ) } );
	}
	void TriangleSES::render( const Object3D::Camera & p_camera ) const
	{
		BaseView3D::render( p_camera );

		for ( const std::pair<const Model::Representation::InstantiatedRepresentation * const,
							  VTX::Representation::RepresentationTarget> & representationData :
			  _model->getCategory()->getMolecule()->getRepresentationData() )
		{
			if ( representationData.first->hasToDrawSES() )
			{
				const Model::Representation::SESData &		triangleData = representationData.first->getSESData();
				const Representation::TargetRange<void *> & target
					= representationData.second.getTrianglesSES( _model->getCategory()->getCategoryEnum() );

				if ( target.indices.size() > 0 )
				{
					_model->getBuffer()->getVao().multiDrawElement( Renderer::GL::VertexArray::DrawMode::TRIANGLES,
																	(GLsizei *)( &target.counts[ 0 ] ),
																	Renderer::GL::VertexArray::Type::UNSIGNED_INT,
																	(GLvoid **)( &target.indices[ 0 ] ),
																	GLsizei( target.indices.size() ) );
				}
			}
		}
	}
} // namespace VTX::View::D3
