#include "blur.hpp"
#include "renderer/gl/gl.hpp"
#include "setting.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Renderer
	{
		namespace Pass
		{
			Blur::~Blur()
			{
				glDeleteFramebuffers( 1, &_fboFirstPass );
				glDeleteTextures( 1, &_textureFirstPass );
				glDeleteFramebuffers( 1, &_fbo );
				glDeleteTextures( 1, &_texture );
			}

			void Blur::init( GLSL::ProgramManager & p_programManager, const uint p_width, const uint p_height )
			{
				// first pass fbo/texture
				glGenFramebuffers( 1, &_fboFirstPass );
				glBindFramebuffer( GL_FRAMEBUFFER, _fboFirstPass );

				glGenTextures( 1, &_textureFirstPass );
				glBindTexture( GL_TEXTURE_2D, _textureFirstPass );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_R16F, p_width, p_height, 0, GL_RED, GL_FLOAT, nullptr );

				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textureFirstPass, 0 );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				glGenFramebuffers( 1, &_fbo );
				glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				glGenTextures( 1, &_texture );
				glBindTexture( GL_TEXTURE_2D, _texture );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_R16F, p_width, p_height, 0, GL_RED, GL_FLOAT, nullptr );

				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0 );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				_program = p_programManager.createProgram( "Blur", { "shading/bilateral_blur.frag" } );

				_uBlurSizeLoc			 = glGetUniformLocation( _program->getId(), "uBlurSize" );
				_uInvDirectionTexSizeLoc = glGetUniformLocation( _program->getId(), "uInvDirectionTexSize" );

				_program->use();
				glUniform1i( _uBlurSizeLoc, Setting::Rendering::aoBlurSize );
			}

			void Blur::resize( const uint p_width, const uint p_height )
			{
				glBindTexture( GL_TEXTURE_2D, _textureFirstPass );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_R16F, p_width, p_height, 0, GL_RED, GL_FLOAT, nullptr );

				glBindTexture( GL_TEXTURE_2D, _texture );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_R16F, p_width, p_height, 0, GL_RED, GL_FLOAT, nullptr );
			}

			void Blur::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				// TODO: clean up !!!!!!!!!!!!!!!
				glBindFramebuffer( GL_FRAMEBUFFER, _fboFirstPass );

				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassSSAO().getTexture() );
				glActiveTexture( GL_TEXTURE1 );
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassLinearizeDepth().getTexture() );

				_program->use();
				// TODO don't update each frame
				glUniform1i( _uBlurSizeLoc, Setting::Rendering::aoBlurSize );
				glUniform2f( _uInvDirectionTexSizeLoc, 1.f / p_renderer.getWidth(), 0.f );

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
				glBindTexture( GL_TEXTURE_2D, _textureFirstPass );
				glActiveTexture( GL_TEXTURE1 );
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassLinearizeDepth().getTexture() );

				glUniform2f( _uInvDirectionTexSizeLoc, 0.f, 1.f / p_renderer.getHeight() );

				glBindVertexArray( p_renderer.getQuadVAO() );

				glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

				glBindVertexArray( 0 );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			}
		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
