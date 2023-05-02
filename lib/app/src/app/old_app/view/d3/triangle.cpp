#include "app/old_app/view/d3/triangle.hpp"
#include "app/component/chemistry/category.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/application/representation/representation_manager.hpp"

namespace VTX::View::D3
{
	Renderer::GL::Program * const Triangle::_createProgram()
	{
		return VTX_PROGRAM_MANAGER().createProgram(
			"Triangle", { FilePath( "triangle/triangle.vert" ), FilePath( "triangle/triangle.frag" ) } );
	}

	void Triangle::render( const App::Component::Render::Camera & p_camera ) const
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
	void TriangleSES::render( const App::Component::Render::Camera & p_camera ) const
	{
		BaseView3D::render( p_camera );

		for ( const std::pair<const App::Application::Representation::InstantiatedRepresentation * const,
							  App::Application::Representation::RepresentationTarget> & representationData :
			  _model->getCategory()->getMolecule()->getRepresentationData() )
		{
			if ( representationData.first->hasToDrawSES() )
			{
				const App::Application::Representation::Primitive::SES &		triangleData = representationData.first->getSESData();
				const App::Application::Representation::TargetRange<void *> & target
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
