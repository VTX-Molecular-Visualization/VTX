#include "renderer/gl/pass/outline.hpp"

namespace VTX::Renderer::GL::Pass
{
	Outline::Outline( const size_t p_width, const size_t p_height, ProgramManager & p_pm )
	{
		out.fbo		= std::make_unique<Framebuffer>();
		out.texture = std::make_unique<Texture2D>(
			p_width, p_height, GL_RGBA16F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR );

		out.fbo->attachTexture( *out.texture, GL_COLOR_ATTACHMENT0 );

		_program = p_pm.createProgram( "Outline", std::vector<FilePath> { "default.vert", "outline.frag" } );
		assert( _program != nullptr );
	}

	void Outline::resize( const size_t p_width, const size_t p_height )
	{
		out.texture->resize( p_width, p_height );
		out.fbo->attachTexture( *out.texture, GL_COLOR_ATTACHMENT0 );
	}

	void Outline::render( VertexArray & p_vao )
	{
		assert( in.textureColor != nullptr );
		assert( in.textureDepth != nullptr );

		out.fbo->bind( GL_DRAW_FRAMEBUFFER );
		in.textureColor->bindToUnit( 0 );
		in.textureDepth->bindToUnit( 1 );
		_program->use();
		p_vao.drawArray( GL_TRIANGLE_STRIP, 0, 4 );
		out.fbo->unbind();
	}
} // namespace VTX::Renderer::GL::Pass
