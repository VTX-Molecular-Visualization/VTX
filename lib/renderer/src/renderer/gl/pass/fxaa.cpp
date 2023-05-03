#include "renderer/gl/pass/fxaa.hpp"

namespace VTX::Renderer::GL::Pass
{
	void FXAA::init( const size_t p_width, const size_t p_height, ProgramManager & p_pm )
	{
		out.texture.create( p_width, p_height, GL_RGBA16F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		_program = p_pm.createProgram( "AA", std::vector<FilePath> { "fxaa.frag" } );
		assert( _program != nullptr );
	}

	void FXAA::resize( const size_t p_width, const size_t p_height ) { out.texture.resize( p_width, p_height ); }

	void FXAA::render( VertexArray & p_vao )
	{
		assert( in.texture != nullptr );

		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
		in.texture->bindToUnit( 1 );
		_program->use();
		p_vao.drawArray( GL_TRIANGLE_STRIP, 0, 4 );
	}

} // namespace VTX::Renderer::GL::Pass
