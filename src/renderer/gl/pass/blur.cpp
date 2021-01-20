#include "blur.hpp"
#include "renderer/gl/gl.hpp"
#include "vtx_app.hpp"

namespace VTX::Renderer::GL::Pass
{
	Blur::~Blur()
	{
		gl()->glDeleteFramebuffers( 1, &_fboFirstPass );
		gl()->glDeleteTextures( 1, &_textureFirstPass );
		gl()->glDeleteFramebuffers( 1, &_fbo );
		gl()->glDeleteTextures( 1, &_texture );
	}

	void Blur::init( const uint p_width, const uint p_height )
	{
		// first pass fbo/texture
		gl()->glCreateFramebuffers( 1, &_fboFirstPass );

		gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_textureFirstPass );
		gl()->glTextureParameteri( _textureFirstPass, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _textureFirstPass, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _textureFirstPass, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		gl()->glTextureParameteri( _textureFirstPass, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		gl()->glTextureStorage2D( _textureFirstPass, 1, GL_R16F, p_width, p_height );

		gl()->glNamedFramebufferTexture( _fboFirstPass, GL_COLOR_ATTACHMENT0, _textureFirstPass, 0 );

		gl()->glCreateFramebuffers( 1, &_fbo );

		gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_texture );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		gl()->glTextureStorage2D( _texture, 1, GL_R16F, p_width, p_height );
		clearTexture();

		gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _texture, 0 );

		_program = VTX_PROGRAM_MANAGER().createProgram( "Blur", { "shading/bilateral_blur.frag" } );

		_uBlurSizeLoc			 = gl()->glGetUniformLocation( _program->getId(), "uBlurSize" );
		_uInvDirectionTexSizeLoc = gl()->glGetUniformLocation( _program->getId(), "uInvDirectionTexSize" );

		_program->use();
		gl()->glUniform1i( _uBlurSizeLoc, VTX_SETTING().aoBlurSize );
	}

	void Blur::resize( const uint p_width, const uint p_height )
	{
		gl()->glDeleteTextures( 1, &_textureFirstPass );
		gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_textureFirstPass );
		gl()->glTextureParameteri( _textureFirstPass, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _textureFirstPass, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _textureFirstPass, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		gl()->glTextureParameteri( _textureFirstPass, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		gl()->glTextureStorage2D( _textureFirstPass, 1, GL_R16F, p_width, p_height );

		gl()->glNamedFramebufferTexture( _fboFirstPass, GL_COLOR_ATTACHMENT0, _textureFirstPass, 0 );

		gl()->glDeleteTextures( 1, &_texture );
		gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_texture );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		gl()->glTextureStorage2D( _texture, 1, GL_R16F, p_width, p_height );
		clearTexture();

		gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _texture, 0 );
	}

	void Blur::render( const Object3D::Scene & p_scene, const GL & p_renderer )
	{
		// TODO: clean up !!!!!!!!!!!!!!!
		gl()->glBindFramebuffer( GL_FRAMEBUFFER, _fboFirstPass );

		gl()->glBindTextureUnit( 0, p_renderer.getPassSSAO().getTexture() );
		gl()->glBindTextureUnit( 1, p_renderer.getPassLinearizeDepth().getTexture() );

		_program->use();
		// TODO don't update each frame
		gl()->glUniform1i( _uBlurSizeLoc, VTX_SETTING().aoBlurSize );
		gl()->glUniform2i( _uInvDirectionTexSizeLoc, 1, 0 );

		gl()->glBindVertexArray( p_renderer.getQuadVAO() );

		gl()->glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

		gl()->glBindVertexArray( 0 );

		gl()->glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		gl()->glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

		gl()->glBindTextureUnit( 0, _textureFirstPass );
		gl()->glBindTextureUnit( 1, p_renderer.getPassLinearizeDepth().getTexture() );

		gl()->glUniform2i( _uInvDirectionTexSizeLoc, 0, 1 );

		gl()->glBindVertexArray( p_renderer.getQuadVAO() );

		gl()->glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

		gl()->glBindVertexArray( 0 );

		gl()->glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	void Blur::clearTexture()
	{
		float clearColor = 1.f;
		gl()->glClearTexImage( _texture, 0, GL_RED, GL_FLOAT, &clearColor );
	}

} // namespace VTX::Renderer::GL::Pass
