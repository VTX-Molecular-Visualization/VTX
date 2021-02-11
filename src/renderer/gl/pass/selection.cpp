#include "selection.hpp"
#include "object3d/camera.hpp"
#include "renderer/gl/gl.hpp"
#include "renderer/gl/program_manager.hpp"
#include "vtx_app.hpp"

namespace VTX::Renderer::GL::Pass
{
	Selection::~Selection()
	{
		gl()->glDeleteFramebuffers( 1, &_fbo );

		gl()->glDeleteTextures( 1, &_texture );
	}

	void Selection::init( const uint p_width, const uint p_height, const GL & p_renderer )
	{
		gl()->glCreateFramebuffers( 1, &_fbo );

		gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_texture );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		gl()->glTextureStorage2D( _texture, 1, GL_RGBA16F, p_width, p_height );

		updateOutputFBO( p_renderer );

		_program = VTX_PROGRAM_MANAGER().createProgram( "Selection", { "shading/selection.frag" } );

		_program->use();

		_uProjMatrixLoc = gl()->glGetUniformLocation( _program->getId(), "uProjMatrix" );
		_uLineColorLoc	= gl()->glGetUniformLocation( _program->getId(), "uLineColor" );

		const Color::Rgb & lineColor = VTX_SETTING().outlineColor;
		gl()->glUniform3f( _uLineColorLoc, lineColor.getR(), lineColor.getG(), lineColor.getB() );
	}

	void Selection::resize( const uint p_width, const uint p_height, const GL & p_renderer )
	{
		gl()->glDeleteTextures( 1, &_texture );
		gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_texture );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		gl()->glTextureStorage2D( _texture, 1, GL_RGBA16F, p_width, p_height );

		updateOutputFBO( p_renderer );
	}

	void Selection::render( const Object3D::Scene & p_scene, const GL & p_renderer )
	{
		if ( VTX_SETTING().activeAA )
		{
			gl()->glBindFramebuffer( GL_FRAMEBUFFER, _fbo );
		}
		else
		{
			gl()->glBindFramebuffer( GL_FRAMEBUFFER, p_renderer.getOutputFbo() );
		}

		gl()->glBindTextureUnit( 0, p_renderer.getPassGeometric().getViewPositionsNormalsCompressedTexture() );
		gl()->glBindTextureUnit( 1,
								 VTX_SETTING().activeOutline ? p_renderer.getPassOutline().getTexture()
															 : p_renderer.getPassShading().getTexture() );
		gl()->glBindTextureUnit( 2, p_renderer.getPassLinearizeDepth().getTexture() );

		_program->use();

		if ( VTXApp::get().MASK & VTX_MASK_CAMERA_UPDATED )
		{
			gl()->glUniformMatrix4fv(
				_uProjMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( ( p_scene.getCamera().getProjectionMatrix() ) ) );
		}

		if ( VTXApp::get().MASK & VTX_MASK_UNIFORM_UPDATED )
		{
			const Color::Rgb lineColor = Color::Rgb( 45, 243, 26 );
			gl()->glUniform3f( _uLineColorLoc, lineColor.getR(), lineColor.getG(), lineColor.getB() );
		}

		gl()->glBindVertexArray( p_renderer.getQuadVAO() );
		gl()->glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		gl()->glBindVertexArray( 0 );

		if ( VTX_SETTING().activeAA == false )
		{
			gl()->glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		}
	}

	void Selection::updateOutputFBO( const GL & p_renderer )
	{
		if ( VTX_SETTING().activeAA )
		{
			gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _texture, 0 );
		}
	}
} // namespace VTX::Renderer::GL::Pass
