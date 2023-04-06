#include "blur.hpp"
#include "filepath.hpp"
#include "gl/gl.hpp"
#include "gl/program_manager.hpp"

namespace VTX::Renderer::GL::Pass
{
	void Blur::init( const size_t p_width, const size_t p_height, const GL & )
	{
		_textureFirstPass.create( p_width,
								  p_height,
								  Wrapper::Texture2D::InternalFormat::R16F,
								  Wrapper::Texture2D::Wrapping::CLAMP_TO_EDGE,
								  Wrapper::Texture2D::Wrapping::CLAMP_TO_EDGE,
								  Wrapper::Texture2D::Filter::NEAREST,
								  Wrapper::Texture2D::Filter::NEAREST );

		_fboFirstPass.create( Wrapper::Framebuffer::Target::DRAW_FRAMEBUFFER );
		_fboFirstPass.attachTexture( _textureFirstPass, Wrapper::Framebuffer::Attachment::COLOR0 );

		_texture.create( p_width,
						 p_height,
						 Wrapper::Texture2D::InternalFormat::R16F,
						 Wrapper::Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Wrapper::Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Wrapper::Texture2D::Filter::NEAREST,
						 Wrapper::Texture2D::Filter::NEAREST );
		clearTexture();

		_fbo.create( Wrapper::Framebuffer::Target::DRAW_FRAMEBUFFER );
		_fbo.attachTexture( _texture, Wrapper::Framebuffer::Attachment::COLOR0 );

		_program = VTX_PROGRAM_MANAGER().createProgram( "Blur", { Util::FilePath( "shading/bilateral_blur.frag" ) } );

		_program->use();
		//_program->setInt( "uBlurSize", VTX_RENDER_EFFECT().getSSAOBlurSize() );
	}

	void Blur::resize( const size_t p_width, const size_t p_height, const GL & )
	{
		_textureFirstPass.resize( p_width, p_height );
		_texture.resize( p_width, p_height );

		clearTexture();

		_fboFirstPass.attachTexture( _textureFirstPass, Wrapper::Framebuffer::Attachment::COLOR0 );
		_fbo.attachTexture( _texture, Wrapper::Framebuffer::Attachment::COLOR0 );
	}

	void Blur::render( const GL & p_renderer )
	{
		_fboFirstPass.bind();
		p_renderer.getPassSSAO().getTexture().bindToUnit( 0 );
		p_renderer.getPassLinearizeDepth().getTexture().bindToUnit( 1 );

		_program->use();

		// if ( VTXApp::get().MASK & VTX_MASK_UNIFORM_UPDATED )
		{
			//_program->setInt( "uBlurSize", VTX_RENDER_EFFECT().getSSAOBlurSize() );
		}

		_program->setVec2i( "uDirection", 1, 0 );

		p_renderer.getQuadVAO().drawArray( Wrapper::VertexArray::DrawMode::TRIANGLE_STRIP, 0, 4 );

		_fbo.bind();

		_textureFirstPass.bindToUnit( 0 );
		p_renderer.getPassLinearizeDepth().getTexture().bindToUnit( 1 );

		_program->setVec2i( "uDirection", 0, 1 );

		p_renderer.getQuadVAO().drawArray( Wrapper::VertexArray::DrawMode::TRIANGLE_STRIP, 0, 4 );
	}

	void Blur::clearTexture()
	{
		const float value = 1.f;
		_texture.clear( &value, Wrapper::Texture2D::Format::RED, Wrapper::Texture2D::Type::FLOAT );
	}

} // namespace VTX::Renderer::GL::Pass
