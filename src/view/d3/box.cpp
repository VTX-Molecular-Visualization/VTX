#include "box.hpp"

namespace VTX::View::D3
{
	Renderer::GL::Program * const Box::_createProgram()
	{
		return VTX_PROGRAM_MANAGER().createProgram( "LineShader", { "line.vert", "line.frag" } );
	}

	void Box::_init() { _gl()->glLineWidth( 1.f ); }

	void Box::render( const Object3D::Camera & p_camera )
	{
		BaseView3D::render( p_camera );

		_gl()->glDrawElements( GL_LINES, uint( _model->getBufferAABBIndices().size() ), GL_UNSIGNED_INT, 0 );
	}
} // namespace VTX::View::D3
