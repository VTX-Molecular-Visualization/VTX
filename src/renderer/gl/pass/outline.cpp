#include "outline.hpp"
#include "renderer/gl/gl.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Renderer
	{
		namespace Pass
		{
			Outline::~Outline()
			{
				OGL().glDeleteFramebuffers( 1, &_fbo );
				OGL().glDeleteTextures( 1, &_texture );
			}

			void Outline::init( GLSL::ProgramManager & p_programManager, const uint p_width, const uint p_height )
			{
				OGL().glGenFramebuffers( 1, &_fbo );
				OGL().glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				OGL().glGenTextures( 1, &_texture );
				OGL().glBindTexture( GL_TEXTURE_2D, _texture );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				OGL().glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, nullptr );

				OGL().glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0 );

				OGL().glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				_program = p_programManager.createProgram( "Outline", { "shading/outline.frag" } );

				_program->use();

				_uProjMatrixLoc = OGL().glGetUniformLocation( _program->getId(), "uProjMatrix" );
				_uLineColorLoc	= OGL().glGetUniformLocation( _program->getId(), "uLineColor" );

				const Color::Rgb & lineColor = VTX_SETTING().outlineColor;
				OGL().glUniform3f( _uLineColorLoc, lineColor.getR(), lineColor.getG(), lineColor.getB() );
			}

			void Outline::resize( const uint p_width, const uint p_height )
			{
				OGL().glBindTexture( GL_TEXTURE_2D, _texture );
				OGL().glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, nullptr );
			}

			void Outline::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				OGL().glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				OGL().glActiveTexture( GL_TEXTURE0 );
				OGL().glBindTexture( GL_TEXTURE_2D, p_renderer.getPassShading().getTexture() );
				OGL().glActiveTexture( GL_TEXTURE1 );
				OGL().glBindTexture( GL_TEXTURE_2D, p_renderer.getPassLinearizeDepth().getTexture() );

				_program->use();

				// TODO: do not update each frame
				OGL().glUniformMatrix4fv( _uProjMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( ( p_scene.getCamera().getProjectionMatrix() ) ) );
				const Color::Rgb & lineColor = VTX_SETTING().outlineColor;
				OGL().glUniform3f( _uLineColorLoc, lineColor.getR(), lineColor.getG(), lineColor.getB() );

				OGL().glBindVertexArray( p_renderer.getQuadVAO() );
				OGL().glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
				OGL().glBindVertexArray( 0 );

				OGL().glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			}
		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
