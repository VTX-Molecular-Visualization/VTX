#include "blur.hpp"
#include "renderer/gl/gl.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Renderer
	{
		namespace Pass
		{
			Blur::~Blur()
			{
				gl()->glDeleteFramebuffers( 1, &_fboFirstPass );
				gl()->glDeleteTextures( 1, &_textureFirstPass );
				gl()->glDeleteFramebuffers( 1, &_fbo );
				gl()->glDeleteTextures( 1, &_texture );
			}

			void Blur::init( GLSL::ProgramManager & p_programManager, const uint p_width, const uint p_height )
			{
				// first pass fbo/texture
				gl()->glGenFramebuffers( 1, &_fboFirstPass );
				gl()->glBindFramebuffer( GL_FRAMEBUFFER, _fboFirstPass );

				gl()->glGenTextures( 1, &_textureFirstPass );
				gl()->glBindTexture( GL_TEXTURE_2D, _textureFirstPass );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				gl()->glTexImage2D( GL_TEXTURE_2D, 0, GL_R16F, p_width, p_height, 0, GL_RED, GL_FLOAT, nullptr );

				gl()->glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textureFirstPass, 0 );

				gl()->glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				gl()->glGenFramebuffers( 1, &_fbo );
				gl()->glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				gl()->glGenTextures( 1, &_texture );
				gl()->glBindTexture( GL_TEXTURE_2D, _texture );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				gl()->glTexImage2D( GL_TEXTURE_2D, 0, GL_R16F, p_width, p_height, 0, GL_RED, GL_FLOAT, nullptr );

				gl()->glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0 );

				gl()->glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				_program = p_programManager.createProgram( "Blur", { "shading/bilateral_blur.frag" } );

				_uBlurSizeLoc			 = gl()->glGetUniformLocation( _program->getId(), "uBlurSize" );
				_uInvDirectionTexSizeLoc = gl()->glGetUniformLocation( _program->getId(), "uInvDirectionTexSize" );

				_program->use();
				gl()->glUniform1i( _uBlurSizeLoc, VTX_SETTING().aoBlurSize );
			}

			void Blur::resize( const uint p_width, const uint p_height )
			{
				gl()->glBindTexture( GL_TEXTURE_2D, _textureFirstPass );
				gl()->glTexImage2D( GL_TEXTURE_2D, 0, GL_R16F, p_width, p_height, 0, GL_RED, GL_FLOAT, nullptr );

				gl()->glBindTexture( GL_TEXTURE_2D, _texture );
				gl()->glTexImage2D( GL_TEXTURE_2D, 0, GL_R16F, p_width, p_height, 0, GL_RED, GL_FLOAT, nullptr );
			}

			void Blur::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				// TODO: clean up !!!!!!!!!!!!!!!
				gl()->glBindFramebuffer( GL_FRAMEBUFFER, _fboFirstPass );

				gl()->glActiveTexture( GL_TEXTURE0 );
				gl()->glBindTexture( GL_TEXTURE_2D, p_renderer.getPassSSAO().getTexture() );
				gl()->glActiveTexture( GL_TEXTURE1 );
				gl()->glBindTexture( GL_TEXTURE_2D, p_renderer.getPassLinearizeDepth().getTexture() );

				_program->use();
				// TODO don't update each frame
				gl()->glUniform1i( _uBlurSizeLoc, VTX_SETTING().aoBlurSize );
				gl()->glUniform2i( _uInvDirectionTexSizeLoc, 1, 0 );

				gl()->glBindVertexArray( p_renderer.getQuadVAO() );

				gl()->glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

				gl()->glBindVertexArray( 0 );

				gl()->glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				gl()->glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				gl()->glActiveTexture( GL_TEXTURE0 );
				gl()->glBindTexture( GL_TEXTURE_2D, 0 );
				gl()->glActiveTexture( GL_TEXTURE1 );
				gl()->glBindTexture( GL_TEXTURE_2D, 0 );

				gl()->glActiveTexture( GL_TEXTURE0 );
				gl()->glBindTexture( GL_TEXTURE_2D, _textureFirstPass );
				gl()->glActiveTexture( GL_TEXTURE1 );
				gl()->glBindTexture( GL_TEXTURE_2D, p_renderer.getPassLinearizeDepth().getTexture() );

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
		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
