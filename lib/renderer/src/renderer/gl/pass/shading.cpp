#include "renderer/gl/pass/shading.hpp"

namespace VTX::Renderer::GL::Pass
{
	void Shading::init( const size_t p_width, const size_t p_height )
	{
		out.texture.create( p_width, p_height, GL_RGBA16F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		out.fbo.create();
		out.fbo.attachTexture( out.texture, GL_COLOR_ATTACHMENT0 );

		/*
		_toonShading
			= VTX_PROGRAM_MANAGER().createProgram( "ToonShading", { IO::FilePath( "shading/shading_toon.frag" ) } );
		_diffuseShading = VTX_PROGRAM_MANAGER().createProgram( "DiffuseShading",
															   { IO::FilePath( "shading/shading_diffuse.frag" ) } );
		_glossyShading
			= VTX_PROGRAM_MANAGER().createProgram( "GlossyShading", { IO::FilePath( "shading/shading_glossy.frag" ) } );
		_flatShading
			= VTX_PROGRAM_MANAGER().createProgram( "FlatShading", { IO::FilePath( "shading/shading_flat.frag" ) } );

		// Use setting value.
		set();
		*/
	}

	void Shading::resize( const size_t p_width, const size_t p_height )
	{
		out.texture.resize( p_width, p_height );

		out.fbo.attachTexture( out.texture, GL_COLOR_ATTACHMENT0 );
	}

	void Shading::render()
	{
		assert( in.textureViewPositionsNormals != nullptr );
		assert( in.texture != nullptr );
		assert( in.textureBlur != nullptr );

		out.fbo.bind( GL_DRAW_FRAMEBUFFER );

		in.textureViewPositionsNormals->bindToUnit( 0 );
		in.texture->bindToUnit( 1 );
		// If SSAO/Blur disabled, texture is previoulsy cleared.
		in.textureBlur->bindToUnit( 2 );

		/*
		_currentShading->use();

		if ( VTXApp::get().MASK & VTX_MASK_UNIFORM_UPDATED )
		{
			const Util::Color::Rgba & bgColor = VTX_RENDER_EFFECT().getBackgroundColor();
			_currentShading->setVec4f( "uBackgroundColor",
									   bgColor.getR(),
									   bgColor.getG(),
									   bgColor.getB(),
									   VTX_SETTING().getSnapshotBackgroundOpacity() );
			_currentShading->setFloat( "uFogNear", VTX_RENDER_EFFECT().getFogNear() );
			_currentShading->setFloat( "uFogFar", VTX_RENDER_EFFECT().getFogFar() );
			_currentShading->setFloat( "uFogDensity",
									   VTX_RENDER_EFFECT().isFogEnabled() ? VTX_RENDER_EFFECT().getFogDensity() : 0.f );
			const Util::Color::Rgba & fogColor = VTX_RENDER_EFFECT().getFogColor();
			_currentShading->setVec3f( "uFogColor", fogColor );

			const Util::Color::Rgba & lightColor = VTX_RENDER_EFFECT().getCameraLightColor();
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
		*/
	}
} // namespace VTX::Renderer::GL::Pass
