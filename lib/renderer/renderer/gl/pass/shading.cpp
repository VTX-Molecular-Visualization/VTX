#include "shading.hpp"
#include "gl/gl.hpp"
#include "gl/program_manager.hpp"

namespace VTX::Renderer::GL::Pass
{
	void Shading::init( const size_t p_width, const size_t p_height, const GL & )
	{
		_texture.create( p_width,
						 p_height,
						 Wrapper::Texture2D::InternalFormat::RGBA16F,
						 Wrapper::Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Wrapper::Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Wrapper::Texture2D::Filter::NEAREST,
						 Wrapper::Texture2D::Filter::NEAREST );

		_fbo.create( Wrapper::Framebuffer::Target::DRAW_FRAMEBUFFER );
		_fbo.attachTexture( _texture, Wrapper::Framebuffer::Attachment::COLOR0 );

		_toonShading
			= VTX_PROGRAM_MANAGER().createProgram( "ToonShading", { Util::FilePath( "shading/shading_toon.frag" ) } );
		_diffuseShading = VTX_PROGRAM_MANAGER().createProgram( "DiffuseShading",
															   { Util::FilePath( "shading/shading_diffuse.frag" ) } );
		_glossyShading	= VTX_PROGRAM_MANAGER().createProgram( "GlossyShading",
															   { Util::FilePath( "shading/shading_glossy.frag" ) } );
		_flatShading
			= VTX_PROGRAM_MANAGER().createProgram( "FlatShading", { Util::FilePath( "shading/shading_flat.frag" ) } );

		// Use setting value.
		set();
	}

	void Shading::resize( const size_t p_width, const size_t p_height, const GL & )
	{
		_texture.resize( p_width, p_height );

		_fbo.attachTexture( _texture, Wrapper::Framebuffer::Attachment::COLOR0 );
	}

	void Shading::render( const GL & p_renderer )
	{
		_fbo.bind();

		p_renderer.getPassGeometric().getViewPositionsNormalsCompressedTexture().bindToUnit( 0 );
		p_renderer.getPassGeometric().getColorsTexture().bindToUnit( 1 );
		// If SSAO/Blur disabled, texture is previoulsy cleared.
		p_renderer.getPassBlur().getTexture().bindToUnit( 2 );

		_currentShading->use();

		/*
		if ( VTXApp::get().MASK & VTX_MASK_UNIFORM_UPDATED )
		{
			const Color::Rgb & bgColor = VTX_RENDER_EFFECT().getBackgroundColor();
			_currentShading->setVec4f( "uBackgroundColor",
									   bgColor.getR(),
									   bgColor.getG(),
									   bgColor.getB(),
									   VTX_SETTING().getSnapshotBackgroundOpacity() );
			_currentShading->setFloat( "uFogNear", VTX_RENDER_EFFECT().getFogNear() );
			_currentShading->setFloat( "uFogFar", VTX_RENDER_EFFECT().getFogFar() );
			_currentShading->setFloat( "uFogDensity",
									   VTX_RENDER_EFFECT().isFogEnabled() ? VTX_RENDER_EFFECT().getFogDensity() : 0.f );
			const Color::Rgb & fogColor = VTX_RENDER_EFFECT().getFogColor();
			_currentShading->setVec3f( "uFogColor", fogColor );

			const Color::Rgb & lightColor = VTX_RENDER_EFFECT().getCameraLightColor();
			_currentShading->setVec3f( "uLightColor", lightColor );
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
		*/
	}

	void Shading::set()
	{
		/*
		switch (VTX_RENDER_EFFECT().getShading())
		{
		case SHADING::TOON: _currentShading = _toonShading; break;
		case SHADING::GLOSSY: _currentShading = _glossyShading; break;
		case SHADING::FLAT_COLOR: _currentShading = _flatShading; break;
		case SHADING::DIFFUSE:
		default: _currentShading = _diffuseShading;
		}

		_currentShading->use();
		*/
	}
} // namespace VTX::Renderer::GL::Pass
