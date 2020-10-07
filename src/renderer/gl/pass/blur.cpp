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
				OGL().glDeleteFramebuffers( 1, &_fboFirstPass );
				OGL().glDeleteTextures( 1, &_textureFirstPass );
				OGL().glDeleteFramebuffers( 1, &_fbo );
				OGL().glDeleteTextures( 1, &_texture );
			}

			void Blur::init( GLSL::ProgramManager & p_programManager, const uint p_width, const uint p_height )
			{
				// first pass fbo/texture
				OGL().glGenFramebuffers( 1, &_fboFirstPass );
				OGL().glBindFramebuffer( GL_FRAMEBUFFER, _fboFirstPass );

				OGL().glGenTextures( 1, &_textureFirstPass );
				OGL().glBindTexture( GL_TEXTURE_2D, _textureFirstPass );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				OGL().glTexImage2D( GL_TEXTURE_2D, 0, GL_R16F, p_width, p_height, 0, GL_RED, GL_FLOAT, nullptr );

				OGL().glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textureFirstPass, 0 );

				OGL().glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				OGL().glGenFramebuffers( 1, &_fbo );
				OGL().glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				OGL().glGenTextures( 1, &_texture );
				OGL().glBindTexture( GL_TEXTURE_2D, _texture );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				OGL().glTexImage2D( GL_TEXTURE_2D, 0, GL_R16F, p_width, p_height, 0, GL_RED, GL_FLOAT, nullptr );

				OGL().glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0 );

				OGL().glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				_program = p_programManager.createProgram( "Blur", { "shading/bilateral_blur.frag" } );

				_uBlurSizeLoc			 = OGL().glGetUniformLocation( _program->getId(), "uBlurSize" );
				_uInvDirectionTexSizeLoc = OGL().glGetUniformLocation( _program->getId(), "uInvDirectionTexSize" );

				_program->use();
				OGL().glUniform1i( _uBlurSizeLoc, VTX_SETTING().aoBlurSize );
			}

			void Blur::resize( const uint p_width, const uint p_height )
			{
				OGL().glBindTexture( GL_TEXTURE_2D, _textureFirstPass );
				OGL().glTexImage2D( GL_TEXTURE_2D, 0, GL_R16F, p_width, p_height, 0, GL_RED, GL_FLOAT, nullptr );

				OGL().glBindTexture( GL_TEXTURE_2D, _texture );
				OGL().glTexImage2D( GL_TEXTURE_2D, 0, GL_R16F, p_width, p_height, 0, GL_RED, GL_FLOAT, nullptr );
			}

			void Blur::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				// TODO: clean up !!!!!!!!!!!!!!!
				OGL().glBindFramebuffer( GL_FRAMEBUFFER, _fboFirstPass );

				OGL().glActiveTexture( GL_TEXTURE0 );
				OGL().glBindTexture( GL_TEXTURE_2D, p_renderer.getPassSSAO().getTexture() );
				OGL().glActiveTexture( GL_TEXTURE1 );
				OGL().glBindTexture( GL_TEXTURE_2D, p_renderer.getPassLinearizeDepth().getTexture() );

				_program->use();
				// TODO don't update each frame
				OGL().glUniform1i( _uBlurSizeLoc, VTX_SETTING().aoBlurSize );
				OGL().glUniform2i( _uInvDirectionTexSizeLoc, 1, 0 );

				OGL().glBindVertexArray( p_renderer.getQuadVAO() );

				OGL().glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

				OGL().glBindVertexArray( 0 );

				OGL().glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				OGL().glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				OGL().glActiveTexture( GL_TEXTURE0 );
				OGL().glBindTexture( GL_TEXTURE_2D, 0 );
				OGL().glActiveTexture( GL_TEXTURE1 );
				OGL().glBindTexture( GL_TEXTURE_2D, 0 );

				OGL().glActiveTexture( GL_TEXTURE0 );
				OGL().glBindTexture( GL_TEXTURE_2D, _textureFirstPass );
				OGL().glActiveTexture( GL_TEXTURE1 );
				OGL().glBindTexture( GL_TEXTURE_2D, p_renderer.getPassLinearizeDepth().getTexture() );

				OGL().glUniform2i( _uInvDirectionTexSizeLoc, 0, 1 );

				OGL().glBindVertexArray( p_renderer.getQuadVAO() );

				OGL().glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

				OGL().glBindVertexArray( 0 );

				OGL().glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			}

			void Blur::clearTexture()
			{
				float clearColor = 1.f;
				OGL().glClearTexImage( _texture, 0, GL_RED, GL_FLOAT, &clearColor );
			}
		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
