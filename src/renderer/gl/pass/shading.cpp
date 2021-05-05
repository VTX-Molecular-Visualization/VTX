#include "shading.hpp"
#include "object3d/camera.hpp"
#include "renderer/gl/gl.hpp"
#include "renderer/gl/program_manager.hpp"
#include "vtx_app.hpp"

namespace VTX::Renderer::GL::Pass
{
	void Shading::init( const uint p_width, const uint p_height, const GL & )
	{
		_texture.create( p_width,
						 p_height,
						 Texture2D::InternalFormat::RGBA16F,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Filter::NEAREST,
						 Texture2D::Filter::NEAREST );

		_fbo.create( Framebuffer::Target::DRAW_FRAMEBUFFER );
		_fbo.attachTexture( _texture, Framebuffer::Attachment::COLOR0 );

		_toonShading	= VTX_PROGRAM_MANAGER().createProgram( "ToonShading", { "shading/shading_toon.frag" } );
		_diffuseShading = VTX_PROGRAM_MANAGER().createProgram( "DiffuseShading", { "shading/shading_diffuse.frag" } );
		_glossyShading	= VTX_PROGRAM_MANAGER().createProgram( "GlossyShading", { "shading/shading_glossy.frag" } );
		_flatShading	= VTX_PROGRAM_MANAGER().createProgram( "FlatShading", { "shading/shading_flat.frag" } );

		// Use setting value.
		set();
	}

	void Shading::resize( const uint p_width, const uint p_height, const GL & )
	{
		_texture.resize( p_width, p_height );

		_fbo.attachTexture( _texture, Framebuffer::Attachment::COLOR0 );
	}

	void Shading::render( const Object3D::Scene & p_scene, const GL & p_renderer )
	{
		_fbo.bind();

		p_renderer.getPassGeometric().getViewPositionsNormalsCompressedTexture().bindToUnit( 0 );
		p_renderer.getPassGeometric().getColorsTexture().bindToUnit( 1 );
		// If SSAO/Blur disabled, texture is previoulsy cleared.
		p_renderer.getPassBlur().getTexture().bindToUnit( 2 );

		_currentShading->use();

		if ( VTXApp::get().MASK & VTX_MASK_UNIFORM_UPDATED )
		{
			const Color::Rgb & bgColor = VTX_SETTING().backgroundColor;
			/// TODO: use a value_ptr ?
			_currentShading->setVec4f(
				"uBackgroundColor", bgColor.getR(), bgColor.getG(), bgColor.getB(), VTX_SETTING().backgroundOpacity );
			_currentShading->setFloat( "uFogNear", VTX_SETTING().fogNear );
			_currentShading->setFloat( "uFogFar", VTX_SETTING().fogFar );
			_currentShading->setFloat( "uFogDensity", VTX_SETTING().activeFog ? VTX_SETTING().fogDensity : 0.f );
			const Color::Rgb & fogColor = VTX_SETTING().fogColor;
			/// TODO: use a value_ptr ?
			_currentShading->setVec3f( "uFogColor", fogColor.getR(), fogColor.getG(), fogColor.getB() );

			const Color::Rgb & lightColor = VTX_SETTING().lightColor;
			/// TODO: use a value_ptr ?
			_currentShading->setVec3f( "uLightColor", lightColor.getR(), lightColor.getG(), lightColor.getB() );
		}

		// TODO: no need for flat shading
		// TODO: let the user choose where's the light
		// TODO: distinguish "view" and "world" lights
		if ( VTXApp::get().MASK & VTX_MASK_CAMERA_UPDATED )
		{
			const Vec4f & lightPosition
				= p_scene.getCamera().getViewMatrix() * Vec4f( p_scene.getCamera().getPosition(), 1.f );
			_currentShading->setVec3f( "uLightPosition", lightPosition );
		}

		p_renderer.getQuadVAO().drawArray( VertexArray::DrawMode::TRIANGLE_STRIP, 0, 4 );
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
	}
} // namespace VTX::Renderer::GL::Pass
