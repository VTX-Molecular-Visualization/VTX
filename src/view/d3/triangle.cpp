#include "triangle.hpp"

namespace VTX::View::D3
{
	Renderer::GL::Program * const Triangle::_createProgram()
	{
		return VTX_PROGRAM_MANAGER().createProgram(
			"Triangle", { IO::FilePath( "triangle/triangle.vert" ), IO::FilePath( "triangle/triangle.frag" ) } );
	}

	void Triangle::render( const Object3D::Camera & p_camera ) const
	{
		BaseView3D::render( p_camera );

		_model->getBuffer()->getVao().drawElement( Renderer::GL::VertexArray::DrawMode::TRIANGLES,
												   GLsizei( _model->getIndices().size() ),
												   Renderer::GL::VertexArray::Type::UNSIGNED_INT );
	}
} // namespace VTX::View::D3
