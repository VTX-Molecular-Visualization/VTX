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
				glGenFramebuffers( 1, &_fboBlur );
				glBindFramebuffer( GL_FRAMEBUFFER, _fboBlur );

				glGenTextures( 1, &_blurTexture );
				glBindTexture( GL_TEXTURE_2D, _blurTexture );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_R8, p_width, p_height, 0, GL_RED, GL_FLOAT, NULL );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _blurTexture, 0 );
				static const GLenum drawBufferBlur[] = { GL_COLOR_ATTACHMENT0 };
				glDrawBuffers( 1, drawBufferBlur );

				_blurShader = p_programManager.createProgram( "Blur", { "shading/blur.frag" } );

				_uBlurSizeLoc	   = glGetUniformLocation( _blurShader->getId(), "uBlurSize" );
				_uBlurSharpnessLoc = glGetUniformLocation( _blurShader->getId(), "uBlurSharpness" );

				_blurShader->use();
				glUniform1i( _uBlurSizeLoc, Setting::Rendering::aoBlurSize );
				glUniform1i( _uBlurSharpnessLoc, Setting::Rendering::aoBlurSharpness );
			}

			void Blur::clean()
			{
				glDeleteFramebuffers( 1, &_fboBlur );
				glDeleteTextures( 1, &_blurTexture );
			}

			void Blur::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				glBindFramebuffer( GL_FRAMEBUFFER, _fboBlur );

				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassSSAO().getSSAOTexture() );
				glActiveTexture( GL_TEXTURE1 );
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassGeometric().getDepthTexture() );

				_blurShader->use();
				// TODO don't update aoBlurSize/aoBlurSharpness each frame
				glUniform1i( _uBlurSizeLoc, Setting::Rendering::aoBlurSize );
				glUniform1i( _uBlurSharpnessLoc, Setting::Rendering::aoBlurSharpness );

				glBindVertexArray( p_renderer.getQuadVAO() );
				glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
				glBindVertexArray( 0 );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			}

		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
