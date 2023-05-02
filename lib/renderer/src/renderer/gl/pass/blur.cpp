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

		_program = p_pm.createProgram( "Blur", { FilePath( "shading/bilateral_blur.frag" ) } );
		assert( _program != nullptr );
		//_program->use();
		//_program->setInt( "uBlurSize", VTX_RENDER_EFFECT().getSSAOBlurSize() );
	}

	void Blur::resize( const size_t p_width, const size_t p_height )
	{
		_textureFirstPass.resize( p_width, p_height );
		out.texture.resize( p_width, p_height );

		clearTexture();

		_fboFirstPass.attachTexture( _textureFirstPass, GL_COLOR_ATTACHMENT0 );
		out.fbo.attachTexture( out.texture, GL_COLOR_ATTACHMENT0 );
	}

	void Blur::render()
	{
		assert( in.textureLinearizeDepth != nullptr );
		assert( in.texture != nullptr );

		_fboFirstPass.bind( GL_DRAW_FRAMEBUFFER );

		in.texture->bindToUnit( 0 );
		in.textureLinearizeDepth->bindToUnit( 1 );

		/*
		_program->use();

		if ( VTXApp::get().MASK & VTX_MASK_UNIFORM_UPDATED )
		{
			_program->setInt( "uBlurSize", VTX_RENDER_EFFECT().getSSAOBlurSize() );
		}

		_program->setVec2i( "uDirection", 1, 0 );

		p_renderer.getQuadVAO().drawArray( VertexArray::DrawMode::TRIANGLE_STRIP, 0, 4 );

		out.fbo.bind(GL_DRAW_FRAMEBUFFER);

		_textureFirstPass.bindToUnit( 0 );
		in.textureLinearizeDepth->bindToUnit( 1 );

		_program->setVec2i( "uDirection", 0, 1 );

		p_renderer.getQuadVAO().drawArray( VertexArray::DrawMode::TRIANGLE_STRIP, 0, 4 );
		*/
	}

	void Blur::clearTexture()
	{
		const float value = 1.f;
		out.texture.clear( &value, GL_RED, GL_FLOAT );
	}

} // namespace VTX::Renderer::GL::Pass
