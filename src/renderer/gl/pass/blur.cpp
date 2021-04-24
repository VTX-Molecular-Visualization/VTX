#include "blur.hpp"
#include "renderer/gl/gl.hpp"
#include "renderer/gl/program_manager.hpp"
#include "vtx_app.hpp"

namespace VTX::Renderer::GL::Pass
{
	void Blur::init( const uint p_width, const uint p_height, const GL & )
	{
		_textureFirstPass.create( p_width,
								  p_height,
								  Texture2D::InternalFormat::R16F,
								  Texture2D::Wrapping::CLAMP_TO_EDGE,
								  Texture2D::Wrapping::CLAMP_TO_EDGE,
								  Texture2D::Filter::NEAREST,
								  Texture2D::Filter::NEAREST );

		_fboFirstPass.create( Framebuffer::Target::DRAW_FRAMEBUFFER );
		_fboFirstPass.attachTexture( _textureFirstPass, Framebuffer::Attachment::COLOR0 );

		_texture.create( p_width,
						 p_height,
						 Texture2D::InternalFormat::R16F,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Filter::NEAREST,
						 Texture2D::Filter::NEAREST );
		clearTexture();

		_fbo.create( Framebuffer::Target::DRAW_FRAMEBUFFER );
		_fbo.attachTexture( _texture, Framebuffer::Attachment::COLOR0 );

		_program = VTX_PROGRAM_MANAGER().createProgram( "Blur", { "shading/bilateral_blur.frag" } );

		_program->use();
		_program->setInt( "uBlurSize", VTX_SETTING().aoBlurSize );
	}

	void Blur::resize( const uint p_width, const uint p_height, const GL & )
	{
		_textureFirstPass.resize( p_width, p_height );
		_texture.resize( p_width, p_height );

		clearTexture();

		_fboFirstPass.attachTexture( _textureFirstPass, Framebuffer::Attachment::COLOR0 );
		_fbo.attachTexture( _texture, Framebuffer::Attachment::COLOR0 );
	}

	void Blur::render( const Object3D::Scene & p_scene, const GL & p_renderer )
	{
		// TODO: clean up !!!!!!!!!!!!!!!
		_fboFirstPass.bind();
		p_renderer.getPassSSAO().getTexture().bindToUnit( 0 );
		p_renderer.getPassLinearizeDepth().getTexture().bindToUnit( 1 );

		_program->use();

		if ( VTXApp::get().MASK & VTX_MASK_UNIFORM_UPDATED )
		{
			_program->setInt( "uBlurSize", VTX_SETTING().aoBlurSize );
		}

		/// TODO: rename uInvDirectionTexSize
		_program->setVec2i( "uInvDirectionTexSize", 1, 0 );

		p_renderer.getQuadVAO().drawArray( VertexArray::DrawMode::TRIANGLE_STRIP, 0, 4 );

		_fbo.bind();

		_textureFirstPass.bindToUnit( 0 );
		p_renderer.getPassLinearizeDepth().getTexture().bindToUnit( 1 );

		/// TODO: rename uInvDirectionTexSize
		_program->setVec2i( "uInvDirectionTexSize", 0, 1 );

		p_renderer.getQuadVAO().drawArray( VertexArray::DrawMode::TRIANGLE_STRIP, 0, 4 );
	}

	void Blur::clearTexture()
	{
		// TODO: wrap it !
		float clearColor = 1.f;
		gl()->glClearTexImage( _texture.getId(), 0, GL_RED, GL_FLOAT, &clearColor );
	}

} // namespace VTX::Renderer::GL::Pass
