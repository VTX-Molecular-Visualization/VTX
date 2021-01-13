#include "linearize_depth.hpp"
#include "renderer/gl/gl.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Renderer
	{
		namespace Pass
		{
			LinearizeDepth::~LinearizeDepth()
			{
				gl()->glDeleteFramebuffers( 1, &_fbo );
				gl()->glDeleteTextures( 1, &_texture );
			}

			void LinearizeDepth::init( GLSL::ProgramManager & p_programManager, const uint p_width, const uint p_height )
			{
				gl()->glCreateFramebuffers( 1, &_fbo );

				gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_texture );
				gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				gl()->glTextureParameteri( _texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				gl()->glTextureParameteri( _texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				gl()->glTextureStorage2D( _texture, 1, GL_R16F, p_width, p_height );

				gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _texture, 0 );
				
				_program = p_programManager.createProgram( "LinearizeDepth", { "shading/linearize_depth.frag" } );

				_uClipInfoLoc = gl()->glGetUniformLocation( _program->getId(), "uClipInfo" );
			}

			void LinearizeDepth::resize( const uint p_width, const uint p_height )
			{
				gl()->glDeleteTextures( 1, &_texture );
				gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_texture );
				gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				gl()->glTextureParameteri( _texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				gl()->glTextureParameteri( _texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				gl()->glTextureStorage2D( _texture, 1, GL_R16F, p_width, p_height );

				gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _texture, 0 );		
			}

			void LinearizeDepth::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				gl()->glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				gl()->glBindTextureUnit( 0, p_renderer.getPassGeometric().getDepthTexture() );

				_program->use();
				// TODO don't update each frame
				const Object3D::Camera & cam	 = VTXApp::get().getScene().getCamera();
				const float				 camNear = cam.getNear();
				const float				 camFar	 = cam.getFar();
				// clipInfo.w: 0 = orhto ; 1 = perspective
				gl()->glUniform4f( _uClipInfoLoc, camNear * camFar, camFar, camFar - camNear, 1.f );

				gl()->glBindVertexArray( p_renderer.getQuadVAO() );

				gl()->glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

				gl()->glBindVertexArray( 0 );

				gl()->glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			}
		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
