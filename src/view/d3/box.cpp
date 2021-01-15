#include "box.hpp"
#include "util/math.hpp"
#include "vtx_app.hpp"

namespace VTX::View::D3
{
	void Box::_init()
	{
		BaseView3D::_init();

		_gl()->glLineWidth( 1.f );
	}

	Renderer::GL::Program * const Box::_createProgram( Renderer::GL::ProgramManager & p_pm )
	{
		return p_pm.createProgram( "LineShader", { "line.vert", "line.frag" } );
	}

	void Box::_render()
	{
		_gl()->glDrawElements( GL_LINES, uint( _model->getBufferAABBIndices().size() ), GL_UNSIGNED_INT, 0 );
	}
} // namespace VTX::View::D3
