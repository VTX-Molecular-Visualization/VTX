#include "shading.hpp"
#include "renderer/gl/gl.hpp"
#include "vtx_app.hpp"

namespace VTX::Renderer::GL::Pass
{
	Shading::~Shading()
	{
		gl()->glDeleteFramebuffers( 1, &_fbo );
		gl()->glDeleteTextures( 1, &_texture );
	}

	void Shading::init( const uint p_width, const uint p_height )
	{
		gl()->glCreateFramebuffers( 1, &_fbo );

		gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_texture );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		gl()->glTextureStorage2D( _texture, 1, GL_RGBA16F, p_width, p_height );

		gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _texture, 0 );

		_toonShading	= VTX_PROGRAM_MANAGER().createProgram( "ToonShading", { "shading/shading_toon.frag" } );
		_diffuseShading = VTX_PROGRAM_MANAGER().createProgram( "DiffuseShading", { "shading/shading_diffuse.frag" } );
		_glossyShading	= VTX_PROGRAM_MANAGER().createProgram( "GlossyShading", { "shading/shading_glossy.frag" } );
		_flatShading	= VTX_PROGRAM_MANAGER().createProgram( "FlatShading", { "shading/shading_flat.frag" } );

		// Use setting value.
		set();
	}

	void Shading::resize( const uint p_width, const uint p_height )
	{
		gl()->glDeleteTextures( 1, &_texture );
		gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_texture );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		gl()->glTextureStorage2D( _texture, 1, GL_RGBA16F, p_width, p_height );

		gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _texture, 0 );
	}

	void Shading::render( const Object3D::Scene & p_scene, const GL & p_renderer )
	{
		gl()->glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

		gl()->glBindTextureUnit( 0, p_renderer.getPassGeometric().getViewPositionsNormalsCompressedTexture() );
		gl()->glBindTextureUnit( 1, p_renderer.getPassGeometric().getColorsTexture() );
		// If SSAO/Blur disabled, texture is previoulsy cleared.
		gl()->glBindTextureUnit( 2, p_renderer.getPassBlur().getTexture() );

		_currentShading->use();

		// TODO: do not update each frame
		const Color::Rgb & bgColor = VTX_SETTING().backgroundColor;
		gl()->glUniform3f( _uBackgroundColorLoc, bgColor.getR(), bgColor.getG(), bgColor.getB() );
		gl()->glUniform1f( _uFogNear, VTX_SETTING().fogNear );
		gl()->glUniform1f( _uFogFar, VTX_SETTING().fogFar );
		gl()->glUniform1f( _uFogDensity, VTX_SETTING().activeFog ? VTX_SETTING().fogDensity : 0.f );
		const Color::Rgb & fogColor = VTX_SETTING().fogColor;
		gl()->glUniform3f( _uFogColor, fogColor.getR(), fogColor.getG(), fogColor.getB() );
		// TODO: no need for flat shading
		// TODO: let the user choose where's the light
		// TODO: distinguish "view" and "world" lights
		const Vec4f & lightPosition
			= p_scene.getCamera().getViewMatrix() * Vec4f( p_scene.getCamera().getPosition(), 1.f );
		gl()->glUniform3f( _uLightPosition, lightPosition.x, lightPosition.y, lightPosition.z );
		const Color::Rgb & lightColor = VTX_SETTING().lightColor;
		gl()->glUniform3f( _uLightColor, lightColor.getR(), lightColor.getG(), lightColor.getB() );

		gl()->glBindVertexArray( p_renderer.getQuadVAO() );
		gl()->glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		gl()->glBindVertexArray( 0 );
		gl()->glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	void Shading::set()
	{
		switch ( VTX_SETTING().shading )
		{
		case SHADING::TOON: _currentShading = _toonShading; break;
		case SHADING::GLOSSY: _currentShading = _glossyShading; break;
		case SHADING::FLAT_COLOR: _currentShading = _flatShading; break;
		case SHADING::DIFFUSE:
		default: _currentShading = _diffuseShading;
		}

		_currentShading->use();
		_uBackgroundColorLoc = gl()->glGetUniformLocation( _currentShading->getId(), "uBackgroundColor" );
		_uFogNear			 = gl()->glGetUniformLocation( _currentShading->getId(), "uFogNear" );
		_uFogFar			 = gl()->glGetUniformLocation( _currentShading->getId(), "uFogFar" );
		_uFogDensity		 = gl()->glGetUniformLocation( _currentShading->getId(), "uFogDensity" );
		_uFogColor			 = gl()->glGetUniformLocation( _currentShading->getId(), "uFogColor" );
		_uLightPosition		 = gl()->glGetUniformLocation( _currentShading->getId(), "uLightPosition" );
		_uLightColor		 = gl()->glGetUniformLocation( _currentShading->getId(), "uLightColor" );
	}
} // namespace VTX::Renderer::GL::Pass
