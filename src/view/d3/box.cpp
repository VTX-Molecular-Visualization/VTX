#include "box.hpp"

namespace VTX::View::D3
{
	Renderer::GL::Program * const Box::_createProgram()
	{
		return VTX_PROGRAM_MANAGER().createProgram( "LineShader",
													{ IO::FilePath( "line.vert" ), IO::FilePath( "line.frag" ) } );
	}

	void Box::_init() { _gl()->glLineWidth( 2.f ); }

	void Box::render( const Object3D::Camera & p_camera ) const
	{
		BaseView3D::render( p_camera );

		_model->getBuffer()->getVao().drawElement( Renderer::GL::VertexArray::DrawMode::LINES,
												   GLsizei( _model->getIndices().size() ),
												   Renderer::GL::VertexArray::Type::UNSIGNED_INT );
	}
} // namespace VTX::View::D3
