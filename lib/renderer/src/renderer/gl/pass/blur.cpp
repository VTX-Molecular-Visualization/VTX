#include "renderer/gl/pass/blur.hpp"

namespace VTX::Renderer::GL::Pass
{
	void Blur::init( const size_t p_width, const size_t p_height, ProgramManager & p_pm )
	{
		_textureFirstPass.create(
			p_width, p_height, GL_R16F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		_fboFirstPass.create();
		_fboFirstPass.attachTexture( _textureFirstPass, GL_COLOR_ATTACHMENT0 );

		out.texture.create( p_width, p_height, GL_R16F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );
		clearTexture();

		out.fbo.create();
		out.fbo.attachTexture( out.texture, GL_COLOR_ATTACHMENT0 );

		_program = p_pm.createProgram( "Blur", std::vector<FilePath> { "bilateral_blur.frag" } );
		assert( _program != nullptr );
	}

	void Blur::resize( const size_t p_width, const size_t p_height )
	{
		_textureFirstPass.resize( p_width, p_height );
		out.texture.resize( p_width, p_height );

		clearTexture();

		_fboFirstPass.attachTexture( _textureFirstPass, GL_COLOR_ATTACHMENT0 );
		out.fbo.attachTexture( out.texture, GL_COLOR_ATTACHMENT0 );
	}

	void Blur::render( VertexArray & p_vao )
	{
		assert( in.textureLinearizeDepth != nullptr );
		assert( in.texture != nullptr );

		// First pass.
		_fboFirstPass.bind( GL_DRAW_FRAMEBUFFER );
		in.texture->bindToUnit( 1 );
		in.textureLinearizeDepth->bindToUnit( 2 );
		_program->use();
		_program->setVec2i( "uDirection", 1, 0 );
		p_vao.drawArray( GL_TRIANGLE_STRIP, 0, 4 );
		_fboFirstPass.unbind();

		// Second pass.
		out.fbo.bind( GL_DRAW_FRAMEBUFFER );
		_textureFirstPass.bindToUnit( 1 );
		in.textureLinearizeDepth->bindToUnit( 2 );
		_program->setVec2i( "uDirection", 0, 1 );
		p_vao.drawArray( GL_TRIANGLE_STRIP, 0, 4 );
		out.fbo.unbind();
	}

	void Blur::clearTexture()
	{
		const float value = 1.f;
		out.texture.clear( &value, GL_RED, GL_FLOAT );
	}

} // namespace VTX::Renderer::GL::Pass
