#include "selection.hpp"
#include "renderer/gl/gl.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Renderer
	{
		namespace Pass
		{
			Selection::~Selection()
			{
				gl()->glDeleteFramebuffers( 1, &_fbo );
				gl()->glDeleteTextures( 1, &_texture );
			}

			void Selection::init( GLSL::ProgramManager & p_programManager, const uint p_width, const uint p_height )
			{
				gl()->glCreateFramebuffers( 1, &_fbo );

				gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_texture );
				gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				gl()->glTextureParameteri( _texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
				gl()->glTextureParameteri( _texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				gl()->glTextureStorage2D( _texture, 1, GL_RGBA16F, p_width, p_height );

				gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _texture, 0 );

				_program = p_programManager.createProgram( "Selection", { "shading/selection.frag" } );

				_program->use();

				_uProjMatrixLoc = gl()->glGetUniformLocation( _program->getId(), "uProjMatrix" );
				_uLineColorLoc	= gl()->glGetUniformLocation( _program->getId(), "uLineColor" );

				const Color::Rgb & lineColor = VTX_SETTING().outlineColor;
				gl()->glUniform3f( _uLineColorLoc, lineColor.getR(), lineColor.getG(), lineColor.getB() );
			}

			void Selection::resize( const uint p_width, const uint p_height )
			{
				gl()->glDeleteTextures( 1, &_texture );
				gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_texture );
				gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				gl()->glTextureParameteri( _texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
				gl()->glTextureParameteri( _texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				gl()->glTextureStorage2D( _texture, 1, GL_RGBA16F, p_width, p_height );

				gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _texture, 0 );
			}

			void Selection::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				gl()->glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				gl()->glBindTextureUnit( 0, p_renderer.getPassGeometric().getViewPositionsNormalsCompressedTexture() );
				gl()->glBindTextureUnit( 1, VTX_SETTING().activeOutline ? p_renderer.getPassOutline().getTexture() : p_renderer.getPassShading().getTexture() );
				gl()->glBindTextureUnit( 2, p_renderer.getPassLinearizeDepth().getTexture() );

				_program->use();

				// TODO: do not update each frame
				gl()->glUniformMatrix4fv( _uProjMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( ( p_scene.getCamera().getProjectionMatrix() ) ) );
				const Color::Rgb lineColor = Color::Rgb( 45, 243, 26 );
				gl()->glUniform3f( _uLineColorLoc, lineColor.getR(), lineColor.getG(), lineColor.getB() );

				gl()->glBindVertexArray( p_renderer.getQuadVAO() );
				gl()->glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
				gl()->glBindVertexArray( 0 );

				gl()->glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			}
		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
