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
				glGenFramebuffers( 1, &_fboAA );
				glBindFramebuffer( GL_FRAMEBUFFER, _fboAA );
				glGenTextures( 1, &_texture );
				glBindTexture( GL_TEXTURE_2D, _texture );
				glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGBA32F, p_width, p_height );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _texture, 0 );
				_aaShader = p_programManager.createProgram( "AA", { "shading/fxaa.frag" } );
			}

			void FXAA::clean()
			{
				glDeleteFramebuffers( 1, &_fboAA );
				glDeleteTextures( 1, &_texture );
			}

			void FXAA::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				glBindFramebuffer( GL_FRAMEBUFFER, _fboAA );

				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassShading().getShadingTexture() );

				_aaShader->use();

				glBindVertexArray( p_renderer.getQuadVAO() );
				glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
				glBindVertexArray( 0 );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			}

		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
