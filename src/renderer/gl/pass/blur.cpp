#include "blur.hpp"
#include "renderer/gl/gl.hpp"
#include "renderer/gl/program_manager.hpp"
#include "vtx_app.hpp"

namespace VTX::Renderer::GL::Pass
{
	Blur::~Blur()
	{
		gl()->glDeleteFramebuffers( 1, &_fboFirstPass );
		gl()->glDeleteFramebuffers( 1, &_fbo );
	}

	void Blur::init( const uint p_width, const uint p_height, const GL & )
	{
		// first pass fbo/texture
		gl()->glCreateFramebuffers( 1, &_fboFirstPass );

		_textureFirstPass.create( p_width,
								  p_height,
								  Texture2D::InternalFormat::R16F,
								  Texture2D::Wrapping::CLAMP_TO_EDGE,
								  Texture2D::Wrapping::CLAMP_TO_EDGE,
								  Texture2D::Filter::NEAREST,
								  Texture2D::Filter::NEAREST );

		gl()->glNamedFramebufferTexture( _fboFirstPass, GL_COLOR_ATTACHMENT0, _textureFirstPass.getId(), 0 );

		gl()->glCreateFramebuffers( 1, &_fbo );

		_texture.create( p_width,
						 p_height,
						 Texture2D::InternalFormat::R16F,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Filter::NEAREST,
						 Texture2D::Filter::NEAREST );
		clearTexture();

		gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _texture.getId(), 0 );

		_program = VTX_PROGRAM_MANAGER().createProgram( "Blur", { "shading/bilateral_blur.frag" } );

		_program->use();
		_program->setInt( "uBlurSize", VTX_SETTING().aoBlurSize );
	}

	void Blur::resize( const uint p_width, const uint p_height, const GL & )
	{
		_textureFirstPass.resize( p_width, p_height );
		_texture.resize( p_width, p_height );

		clearTexture();

		gl()->glNamedFramebufferTexture( _fboFirstPass, GL_COLOR_ATTACHMENT0, _textureFirstPass.getId(), 0 );
		gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _texture.getId(), 0 );
	}

	void Blur::render( const Object3D::Scene & p_scene, const GL & p_renderer )
	{
		// TODO: clean up !!!!!!!!!!!!!!!
		gl()->glBindFramebuffer( GL_FRAMEBUFFER, _fboFirstPass );

		gl()->glBindTextureUnit( 0, p_renderer.getPassSSAO().getTexture() );
		gl()->glBindTextureUnit( 1, p_renderer.getPassLinearizeDepth().getTexture() );

		_program->use();

		if ( VTXApp::get().MASK & VTX_MASK_UNIFORM_UPDATED )
		{
			_program->setInt( "uBlurSize", VTX_SETTING().aoBlurSize );
		}

		/// TODO: rename uInvDirectionTexSize
		_program->setVec2i( "uInvDirectionTexSize", 1, 0 );

		gl()->glBindVertexArray( p_renderer.getQuadVAO() );

		gl()->glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

		gl()->glBindVertexArray( 0 );

		gl()->glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		gl()->glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

		gl()->glBindTextureUnit( 0, _textureFirstPass.getId() );
		gl()->glBindTextureUnit( 1, p_renderer.getPassLinearizeDepth().getTexture() );

		/// TODO: rename uInvDirectionTexSize
		_program->setVec2i( "uInvDirectionTexSize", 0, 1 );

		gl()->glBindVertexArray( p_renderer.getQuadVAO() );

		gl()->glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

		gl()->glBindVertexArray( 0 );

		gl()->glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	void Blur::clearTexture()
	{
		// TODO: wrap it !
		float clearColor = 1.f;
		gl()->glClearTexImage( _texture.getId(), 0, GL_RED, GL_FLOAT, &clearColor );
	}

} // namespace VTX::Renderer::GL::Pass
