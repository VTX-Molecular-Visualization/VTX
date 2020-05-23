#include "blur.hpp"
#include "renderer/gl/gl.hpp"
#include "setting.hpp"

namespace VTX
{
	namespace Renderer
	{
		namespace Pass
		{
			void Blur::init( GLSL::ProgramManager & p_programManager, const uint p_width, const uint p_height )
			{
				// first pass fbo/texture
				glGenFramebuffers( 1, &_fboTmp );
				glBindFramebuffer( GL_FRAMEBUFFER, _fboTmp );

				glGenTextures( 1, &_textureTmp );
				glBindTexture( GL_TEXTURE_2D, _textureTmp );
				glTexStorage2D( GL_TEXTURE_2D, 1, GL_R16F, p_width, p_height );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textureTmp, 0 );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				glGenFramebuffers( 1, &_fbo );
				glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				glGenTextures( 1, &_texture );
				glBindTexture( GL_TEXTURE_2D, _texture );
				glTexStorage2D( GL_TEXTURE_2D, 1, GL_R16F, p_width, p_height );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0 );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				_blurShader = p_programManager.createProgram( "Blur", { "shading/bilateral_blur.frag" } );

				_uBlurSizeLoc	   = glGetUniformLocation( _blurShader->getId(), "uBlurSize" );
				_uBlurSharpnessLoc = glGetUniformLocation( _blurShader->getId(), "uBlurSharpness" );
				_uInvTexSize	   = glGetUniformLocation( _blurShader->getId(), "uInvTexSize" );

				_blurShader->use();
				glUniform1i( _uBlurSizeLoc, Setting::Rendering::aoBlurSize );
				glUniform1i( _uBlurSharpnessLoc, Setting::Rendering::aoBlurSharpness );
			}

			void Blur::clean()
			{
				glDeleteFramebuffers( 1, &_fboTmp );
				glDeleteTextures( 1, &_textureTmp );
				glDeleteFramebuffers( 1, &_fbo );
				glDeleteTextures( 1, &_texture );
			}

			void Blur::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				// TODO: clean up !!!!!!!!!!!!!!!
				glBindFramebuffer( GL_FRAMEBUFFER, _fboTmp );

				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassSSAO().getSSAOTexture() );
				glActiveTexture( GL_TEXTURE1 );
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassGeometric().getDepthTexture() );

				_blurShader->use();
				// TODO don't update blurSize/blurSharpness/InvTexSize each frame
				glUniform1i( _uBlurSizeLoc, Setting::Rendering::aoBlurSize );
				glUniform1i( _uBlurSharpnessLoc, Setting::Rendering::aoBlurSharpness );
				glUniform2f( _uInvTexSize, 1.f / p_renderer.getWidth(), 0.f );

				glBindVertexArray( p_renderer.getQuadVAO() );

				glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

				glBindVertexArray( 0 );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D, 0 );
				glActiveTexture( GL_TEXTURE1 );
				glBindTexture( GL_TEXTURE_2D, 0 );

				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D, _textureTmp );
				glActiveTexture( GL_TEXTURE1 );
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassGeometric().getDepthTexture() );

				glUniform2f( _uInvTexSize, 0.f, 1.f / p_renderer.getHeight() );

				glBindVertexArray( p_renderer.getQuadVAO() );

				glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

				glBindVertexArray( 0 );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			}
		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
