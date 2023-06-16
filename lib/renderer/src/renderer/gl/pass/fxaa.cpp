#include "renderer/gl/pass/fxaa.hpp"

namespace VTX::Renderer::GL::Pass
{
	void FXAA::init( const size_t p_width, const size_t p_height, ProgramManager & p_pm )
	{
		out.texture.create( p_width, p_height, GL_RGBA16F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		out.fbo.create();
		out.fbo.attachTexture( out.texture, GL_COLOR_ATTACHMENT0 );

		_program = p_pm.createProgram( "AA", std::vector<FilePath> { "default.vert", "fxaa.frag" } );
		assert( _program != nullptr );
	}

	void FXAA::resize( const size_t p_width, const size_t p_height )
	{
		out.texture.resize( p_width, p_height );
		out.fbo.attachTexture( out.texture, GL_COLOR_ATTACHMENT0 );
	}

	void FXAA::render( VertexArray & p_vao )
	{
		assert( in.textureColor != nullptr );

		out.fbo.bind( GL_DRAW_FRAMEBUFFER );
		in.textureColor->bindToUnit( 0 );
		_program->use();
		p_vao.drawArray( GL_TRIANGLE_STRIP, 0, 4 );
		out.fbo.unbind();
	}

} // namespace VTX::Renderer::GL::Pass
