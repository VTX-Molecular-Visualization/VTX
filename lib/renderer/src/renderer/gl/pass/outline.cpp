#include "renderer/gl/pass/outline.hpp"

namespace VTX::Renderer::GL::Pass
{
	void Outline::init( const size_t p_width, const size_t p_height, ProgramManager & p_pm )
	{
		out.texture.create( p_width, p_height, GL_RGBA16F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR );

		out.fbo.create();
		out.fbo.attachTexture( out.texture, GL_COLOR_ATTACHMENT0 );

		_program = p_pm.createProgram( "Outline", std::vector<FilePath> { "outline.frag" } );
		assert( _program != nullptr );
	}

	void Outline::resize( const size_t p_width, const size_t p_height )
	{
		out.texture.resize( p_width, p_height );

		out.fbo.attachTexture( out.texture, GL_COLOR_ATTACHMENT0 );
	}

	void Outline::render( VertexArray & p_vao )
	{
		assert( in.texture != nullptr );
		assert( in.textureLinearizeDepth != nullptr );

		out.fbo.bind( GL_DRAW_FRAMEBUFFER );
		in.texture->bind( 0 );
		in.textureLinearizeDepth->bind( 1 );
		_program->use();
		p_vao.drawArray( GL_TRIANGLE_STRIP, 0, 4 );
		out.fbo.unbind();
	}
} // namespace VTX::Renderer::GL::Pass
