#include "outline.hpp"
#include "object3d/camera.hpp"
#include "renderer/gl/gl.hpp"
#include "renderer/gl/program_manager.hpp"
#include "vtx_app.hpp"

namespace VTX::Renderer::GL::Pass
{
	Outline::~Outline()
	{
		gl()->glDeleteFramebuffers( 1, &_fbo );
		gl()->glDeleteTextures( 1, &_texture );
	}

	void Outline::init( const uint p_width, const uint p_height, const GL & )
	{
		gl()->glCreateFramebuffers( 1, &_fbo );

		gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_texture );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		gl()->glTextureStorage2D( _texture, 1, GL_RGBA16F, p_width, p_height );

		gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _texture, 0 );

		_program = VTX_PROGRAM_MANAGER().createProgram( "Outline", { "shading/outline.frag" } );

		_program->use();

		_uProjMatrixLoc = _program->getUniformLocation( "uProjMatrix" );
		_uLineColorLoc	= _program->getUniformLocation( "uLineColor" );

		const Color::Rgb & lineColor = VTX_SETTING().outlineColor;
		gl()->glUniform3f( _uLineColorLoc, lineColor.getR(), lineColor.getG(), lineColor.getB() );
	}

	void Outline::resize( const uint p_width, const uint p_height, const GL & )
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

	void Outline::render( const Object3D::Scene & p_scene, const GL & p_renderer )
	{
		gl()->glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

		gl()->glBindTextureUnit( 0, p_renderer.getPassShading().getTexture() );
		gl()->glBindTextureUnit( 1, p_renderer.getPassLinearizeDepth().getTexture() );

		_program->use();

		if ( VTXApp::get().MASK & VTX_MASK_CAMERA_UPDATED )
		{
			gl()->glUniformMatrix4fv(
				_uProjMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( ( p_scene.getCamera().getProjectionMatrix() ) ) );
		}

		if ( VTXApp::get().MASK & VTX_MASK_UNIFORM_UPDATED )
		{
			const Color::Rgb & lineColor = VTX_SETTING().outlineColor;
			gl()->glUniform3f( _uLineColorLoc, lineColor.getR(), lineColor.getG(), lineColor.getB() );
		}

		gl()->glBindVertexArray( p_renderer.getQuadVAO() );
		gl()->glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		gl()->glBindVertexArray( 0 );

		gl()->glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}
} // namespace VTX::Renderer::GL::Pass
