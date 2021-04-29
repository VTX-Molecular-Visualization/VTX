#include "box.hpp"

namespace VTX::View::D3
{
	Renderer::GL::Program * const Box::_createProgram()
	{
		return VTX_PROGRAM_MANAGER().createProgram( "LineShader", { "line.vert", "line.frag" } );
	}

	void Box::_init() { _gl()->glLineWidth( 1.f ); }

	void Box::render( const Object3D::Camera & p_camera ) const
	{
		BaseView3D::render( p_camera );

		_model->getBuffer()->getAABBVao().drawElement( Renderer::GL::VertexArray::DrawMode::LINES,
													   GLsizei( _model->getBufferAABBIndices().size() ),
													   Renderer::GL::VertexArray::Type::UNSIGNED_INT );
	}
} // namespace VTX::View::D3
