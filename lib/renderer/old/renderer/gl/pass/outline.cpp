#include "outline.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "object3d/camera.hpp"
#include "renderer/gl/gl.hpp"
#include "renderer/gl/program_manager.hpp"
#include "vtx_app.hpp"

namespace VTX::Renderer::GL::Pass
{
	void Outline::init( const uint p_width, const uint p_height, const GL & )
	{
		_texture.create( p_width,
						 p_height,
						 Texture2D::InternalFormat::RGBA16F,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Filter::LINEAR,
						 Texture2D::Filter::LINEAR );

		_fbo.create( Framebuffer::Target::DRAW_FRAMEBUFFER );
		_fbo.attachTexture( _texture, Framebuffer::Attachment::COLOR0 );

		_program = VTX_PROGRAM_MANAGER().createProgram( "Outline", { IO::FilePath( "shading/outline.frag" ) } );

		_program->use();

		const Util::Color::Rgba & lineColor = VTX_RENDER_EFFECT().getOutlineColor();
		_program->setVec4f( "uLineColor", lineColor );
		_program->setInt( "uThickness", VTX_RENDER_EFFECT().getOutlineThickness() );
		_program->setFloat( "uSeensivity", VTX_RENDER_EFFECT().getOutlineSensivity() );
	}

	void Outline::resize( const uint p_width, const uint p_height, const GL & )
	{
		_texture.resize( p_width, p_height );

		_fbo.attachTexture( _texture, Framebuffer::Attachment::COLOR0 );
	}

	void Outline::render( const App::Application::Scene & p_scene, const GL & p_renderer )
	{
		_fbo.bind();

		p_renderer.getPassShading().getTexture().bindToUnit( 0 );
		p_renderer.getPassLinearizeDepth().getTexture().bindToUnit( 1 );

		_program->use();

		if ( VTXApp::get().MASK & Render::VTX_MASK_UNIFORM_UPDATED )
		{
			const Util::Color::Rgba & lineColor = VTX_RENDER_EFFECT().getOutlineColor();
			_program->setVec4f( "uLineColor", lineColor );
			_program->setInt( "uThickness", VTX_RENDER_EFFECT().getOutlineThickness() );
			_program->setFloat( "uSensivity", VTX_RENDER_EFFECT().getOutlineSensivity() );
		}

		p_renderer.getQuadVAO().drawArray( VertexArray::DrawMode::TRIANGLE_STRIP, 0, 4 );
	}
} // namespace VTX::Renderer::GL::Pass
