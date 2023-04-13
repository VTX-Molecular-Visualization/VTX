#include "shading.hpp"
#include "old_app/model/renderer/render_effect_preset.hpp"
#include "old_app/object3d/camera.hpp"
#include "old_app/renderer/gl/gl.hpp"
#include "old_app/renderer/gl/program_manager.hpp"
#include "old_app/vtx_app.hpp"

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

		_toonShading
			= VTX_PROGRAM_MANAGER().createProgram( "ToonShading", { FilePath( "shading/shading_toon.frag" ) } );
		_diffuseShading = VTX_PROGRAM_MANAGER().createProgram( "DiffuseShading",
															   { FilePath( "shading/shading_diffuse.frag" ) } );
		_glossyShading
			= VTX_PROGRAM_MANAGER().createProgram( "GlossyShading", { FilePath( "shading/shading_glossy.frag" ) } );
		_flatShading
			= VTX_PROGRAM_MANAGER().createProgram( "FlatShading", { FilePath( "shading/shading_flat.frag" ) } );

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
			const Color::Rgba & bgColor = VTX_RENDER_EFFECT().getBackgroundColor();
			_currentShading->setVec4f( "uBackgroundColor",
									   bgColor.getR(),
									   bgColor.getG(),
									   bgColor.getB(),
									   VTX_SETTING().getSnapshotBackgroundOpacity() );
			_currentShading->setFloat( "uFogNear", VTX_RENDER_EFFECT().getFogNear() );
			_currentShading->setFloat( "uFogFar", VTX_RENDER_EFFECT().getFogFar() );
			_currentShading->setFloat( "uFogDensity",
									   VTX_RENDER_EFFECT().isFogEnabled() ? VTX_RENDER_EFFECT().getFogDensity() : 0.f );
			const Color::Rgba & fogColor = VTX_RENDER_EFFECT().getFogColor();
			_currentShading->setVec3f( "uFogColor", fogColor );

			const Color::Rgba & lightColor = VTX_RENDER_EFFECT().getCameraLightColor();
			_currentShading->setVec3f( "uLightColor", lightColor );
		}

		// TODO: no need for flat shading
		// TODO: let the user choose where's the light
		// TODO: distinguish "view" and "world" lights
		if ( VTXApp::get().MASK & VTX_MASK_CAMERA_UPDATED )
		{
			_currentShading->setBool( "uIsPerspective", p_scene.getCamera().isPerspective() );
		}

		p_renderer.getQuadVAO().drawArray( VertexArray::DrawMode::TRIANGLE_STRIP, 0, 4 );
	}

	void Shading::set()
	{
		switch ( VTX_RENDER_EFFECT().getShading() )
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
