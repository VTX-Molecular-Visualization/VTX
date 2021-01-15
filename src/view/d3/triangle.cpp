#include "triangle.hpp"

namespace VTX::View::D3
{
	Renderer::GL::Program * const Triangle::_createProgram( Renderer::GL::ProgramManager & p_pm )
	{
		return p_pm.createProgram( "Triangle", { "triangle.vert", "triangle.frag" } );
	}

	void Triangle::_render()
	{
		_gl()->glDrawElements( GL_TRIANGLES, uint( _model->getIndices().size() ), GL_UNSIGNED_INT, 0 );
	}
} // namespace VTX::View::D3
