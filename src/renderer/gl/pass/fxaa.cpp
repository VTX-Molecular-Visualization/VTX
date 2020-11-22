#include "fxaa.hpp"
#include "renderer/gl/gl.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Renderer
	{
		namespace Pass
		{
			FXAA::~FXAA()
			{
				gl()->glDeleteFramebuffers( 1, &_fbo );
				gl()->glDeleteTextures( 1, &_texture );
			}

			void FXAA::init( GLSL::ProgramManager & p_programManager, const uint p_width, const uint p_height )
			{
				gl()->glGenFramebuffers( 1, &_fbo );
				gl()->glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				gl()->glGenTextures( 1, &_texture );
				gl()->glBindTexture( GL_TEXTURE_2D, _texture );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				gl()->glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, nullptr );

				gl()->glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0 );

				gl()->glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				_program = p_programManager.createProgram( "AA", { "shading/fxaa.frag" } );
			}

			void FXAA::resize( const uint p_width, const uint p_height )
			{
				gl()->glBindTexture( GL_TEXTURE_2D, _texture );
				gl()->glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, nullptr );
			}

			void FXAA::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				gl()->glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				gl()->glActiveTexture( GL_TEXTURE0 );
				gl()->glBindTexture( GL_TEXTURE_2D, p_renderer.getPassSelection().getTexture() );

				_program->use();

				gl()->glBindVertexArray( p_renderer.getQuadVAO() );
				gl()->glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
				gl()->glBindVertexArray( 0 );

				gl()->glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			}

		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
