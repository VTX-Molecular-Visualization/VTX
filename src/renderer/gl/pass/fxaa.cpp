#include "fxaa.hpp"
#include "renderer/gl/gl.hpp"

namespace VTX
{
	namespace Renderer
	{
		namespace Pass
		{
			void FXAA::init( GLSL::ProgramManager & p_programManager, const uint p_width, const uint p_height )
			{
				glGenFramebuffers( 1, &_fbo );
				glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				glGenTextures( 1, &_texture );
				glBindTexture( GL_TEXTURE_2D, _texture );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, nullptr );

				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0 );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				_program = p_programManager.createProgram( "AA", { "shading/fxaa.frag" } );
			}

			void FXAA::resize( const uint p_width, const uint p_height )
			{
				glBindTexture( GL_TEXTURE_2D, _texture );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, nullptr );
			}

			void FXAA::clean()
			{
				glDeleteFramebuffers( 1, &_fbo );
				glDeleteTextures( 1, &_texture );
			}

			void FXAA::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D,
							   Setting::Rendering::useOutline ? p_renderer.getPassOutline().getTexture()
															  : p_renderer.getPassShading().getTexture() );

				_program->use();

				glBindVertexArray( p_renderer.getQuadVAO() );
				glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
				glBindVertexArray( 0 );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			}

		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
