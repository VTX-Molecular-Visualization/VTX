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
				OGL().glDeleteFramebuffers( 1, &_fbo );
				OGL().glDeleteTextures( 1, &_texture );
			}

			void FXAA::init( GLSL::ProgramManager & p_programManager, const uint p_width, const uint p_height )
			{
				OGL().glGenFramebuffers( 1, &_fbo );
				OGL().glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				OGL().glGenTextures( 1, &_texture );
				OGL().glBindTexture( GL_TEXTURE_2D, _texture );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				OGL().glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, nullptr );

				OGL().glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0 );

				OGL().glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				_program = p_programManager.createProgram( "AA", { "shading/fxaa.frag" } );
			}

			void FXAA::resize( const uint p_width, const uint p_height )
			{
				OGL().glBindTexture( GL_TEXTURE_2D, _texture );
				OGL().glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, nullptr );
			}

			void FXAA::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				OGL().glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				OGL().glActiveTexture( GL_TEXTURE0 );
				OGL().glBindTexture( GL_TEXTURE_2D, VTX_SETTING().activeOutline ? p_renderer.getPassOutline().getTexture() : p_renderer.getPassShading().getTexture() );

				_program->use();

				OGL().glBindVertexArray( p_renderer.getQuadVAO() );
				OGL().glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
				OGL().glBindVertexArray( 0 );

				OGL().glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			}

		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
