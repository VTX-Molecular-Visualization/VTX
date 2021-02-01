#include "triangle.hpp"

namespace VTX::View::D3
{
	Renderer::GL::Program * const Triangle::_createProgram()
	{
		return VTX_PROGRAM_MANAGER().createProgram( "Triangle", { "triangle.vert", "triangle.frag" } );
	}

	void Triangle::render( const Object3D::Camera & p_camera )
	{
		BaseView3D::render( p_camera );

		_gl()->glDrawElements( GL_TRIANGLES, uint( _model->getIndices().size() ), GL_UNSIGNED_INT, 0 );
	}
} // namespace VTX::View::D3
