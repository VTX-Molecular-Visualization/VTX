#include "linearize_depth.hpp"
#include "renderer/gl/gl.hpp"
#include "setting.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Renderer
	{
		namespace Pass
		{
			LinearizeDepth::~LinearizeDepth()
			{
				glDeleteFramebuffers( 1, &_fbo );
				glDeleteTextures( 1, &_texture );
			}

			void LinearizeDepth::init( GLSL::ProgramManager & p_programManager,
									   const uint			  p_width,
									   const uint			  p_height )
			{
				glGenFramebuffers( 1, &_fbo );
				glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				glGenTextures( 1, &_texture );
				glBindTexture( GL_TEXTURE_2D, _texture );
				// TODO: LINEAR useful ?
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_R16F, p_width, p_height, 0, GL_RED, GL_FLOAT, nullptr );

				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0 );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				_program = p_programManager.createProgram( "LinearizeDepth", { "shading/linearize_depth.frag" } );

				_uClipInfoLoc = glGetUniformLocation( _program->getId(), "uClipInfo" );
			}

			void LinearizeDepth::resize( const uint p_width, const uint p_height )
			{
				glBindTexture( GL_TEXTURE_2D, _texture );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_R16F, p_width, p_height, 0, GL_RED, GL_FLOAT, nullptr );
			}

			void LinearizeDepth::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassGeometric().getDepthTexture() );

				_program->use();
				// TODO don't update each frame
				const Object3D::Camera & cam	 = VTXApp::get().getScene().getCamera();
				const float				 camNear = cam.getNear();
				const float				 camFar	 = cam.getFar();
				// clipInfo.w: 0 = orhto ; 1 = perspective
				glUniform4f( _uClipInfoLoc, camNear * camFar, camFar, camFar - camNear, 1.f );

				glBindVertexArray( p_renderer.getQuadVAO() );

				glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

				glBindVertexArray( 0 );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			}
		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
