#include "outline.hpp"
#include "renderer/gl/gl.hpp"
#include "setting.hpp"

namespace VTX
{
	namespace Renderer
	{
		namespace Pass
		{
			Outline::~Outline()
			{
				glDeleteFramebuffers( 1, &_fbo );
				glDeleteTextures( 1, &_texture );
			}

			void Outline::init( GLSL::ProgramManager & p_programManager, const uint p_width, const uint p_height )
			{
				glGenFramebuffers( 1, &_fbo );
				glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				glGenTextures( 1, &_texture );
				glBindTexture( GL_TEXTURE_2D, _texture );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, nullptr );

				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0 );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				_program = p_programManager.createProgram( "Outline", { "shading/outline.frag" } );

				_program->use();

				_uProjMatrixLoc = glGetUniformLocation( _program->getId(), "uProjMatrix" );
				_uLineColorLoc	= glGetUniformLocation( _program->getId(), "uLineColor" );

				const Color::Rgb & lineColor = Setting::Rendering::outlineColor;
				glUniform3f( _uLineColorLoc, lineColor.getR(), lineColor.getG(), lineColor.getB() );
			}

			void Outline::resize( const uint p_width, const uint p_height )
			{
				glBindTexture( GL_TEXTURE_2D, _texture );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, nullptr );
			}

			void Outline::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassShading().getTexture() );
				glActiveTexture( GL_TEXTURE1 );
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassLinearizeDepth().getTexture() );

				_program->use();

				// TODO: do not update each frame
				glUniformMatrix4fv( _uProjMatrixLoc,
									1,
									GL_FALSE,
									Util::Math::value_ptr( ( p_scene.getCamera().getProjectionMatrix() ) ) );
				const Color::Rgb & lineColor = Setting::Rendering::outlineColor;
				glUniform3f( _uLineColorLoc, lineColor.getR(), lineColor.getG(), lineColor.getB() );

				glBindVertexArray( p_renderer.getQuadVAO() );
				glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
				glBindVertexArray( 0 );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			}
		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
