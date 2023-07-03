#include "renderer/gl/pass/blur.hpp"

namespace VTX::Renderer::GL::Pass
{
	Blur::Blur( const size_t p_width, const size_t p_height, ProgramManager & p_pm )
	{
		_fboFirstPass	  = std::make_unique<Framebuffer>();
		_textureFirstPass = std::make_unique<Texture2D>(
			p_width, p_height, GL_R16F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		_fboFirstPass->attachTexture( *_textureFirstPass, GL_COLOR_ATTACHMENT0 );

		_ubo = std::make_unique<Buffer>( uniforms, GL_DYNAMIC_DRAW );

		out.fbo		= std::make_unique<Framebuffer>();
		out.texture = std::make_unique<Texture2D>(
			p_width, p_height, GL_R16F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		clearTexture();
		out.fbo->attachTexture( *out.texture, GL_COLOR_ATTACHMENT0 );

		_program = p_pm.createProgram( "Blur", std::vector<FilePath> { "default.vert", "bilateral_blur.frag" } );
		assert( _program != nullptr );
	}

	void Blur::resize( const size_t p_width, const size_t p_height )
	{
		_textureFirstPass->resize( p_width, p_height );
		out.texture->resize( p_width, p_height );

		clearTexture();

		_fboFirstPass->attachTexture( *_textureFirstPass, GL_COLOR_ATTACHMENT0 );
		out.fbo->attachTexture( *out.texture, GL_COLOR_ATTACHMENT0 );
	}

	void Blur::render( VertexArray & p_vao )
	{
		assert( in.textureDepth != nullptr );
		assert( in.textureColor != nullptr );

		// First pass.
		_fboFirstPass->bind( GL_DRAW_FRAMEBUFFER );
		in.textureColor->bindToUnit( 0 );
		in.textureDepth->bindToUnit( 1 );
		_ubo->bind( GL_UNIFORM_BUFFER, 2 );
		//_ubo->setSub( Vec2i( 1, 0 ), offsetof( StructUniforms, direction ), sizeof( Vec2i ) );
		_program->use();
		_program->setVec2i( "uDirection", 1, 0 );
		p_vao.drawArray( GL_TRIANGLE_STRIP, 0, 4 );
		in.textureColor->unbindFromUnit( 0 );
		_fboFirstPass->unbind();

		// Second pass.
		out.fbo->bind( GL_DRAW_FRAMEBUFFER );
		_textureFirstPass->bindToUnit( 0 );
		_program->setVec2i( "uDirection", 0, 1 );
		//_ubo->setSub( Vec2i( 0, 1 ), offsetof( StructUniforms, direction ), sizeof( Vec2i ) );
		p_vao.drawArray( GL_TRIANGLE_STRIP, 0, 4 );
		_textureFirstPass->unbindFromUnit( 0 );
		in.textureDepth->unbindFromUnit( 1 );
		_ubo->unbind( 2 );
		out.fbo->unbind();
	}

	void Blur::clearTexture()
	{
		const float value = 1.f;
		out.texture->clear( &value, GL_RED, GL_FLOAT );
	}

	void Blur::setSize( const float p_size )
	{
		uniforms.size = p_size;
		_ubo->setSub( p_size, offsetof( StructUniforms, size ), sizeof( float ) );
	}

} // namespace VTX::Renderer::GL::Pass
