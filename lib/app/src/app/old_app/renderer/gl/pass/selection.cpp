#include "app/old_app/renderer/gl/pass/selection.hpp"
#include "app/application/render_effect/render_effect_preset.hpp"
#include "app/component/render/camera.hpp"
#include "app/old_app/renderer/gl/gl.hpp"
#include "app/old_app/renderer/gl/program_manager.hpp"
#include "app/old_app/vtx_app.hpp"

namespace VTX::Renderer::GL::Pass
{
	void Selection::init( const uint p_width, const uint p_height, const GL & p_renderer )
	{
		_texture.create( p_width,
						 p_height,
						 Texture2D::InternalFormat::RGBA16F,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Filter::LINEAR,
						 Texture2D::Filter::LINEAR );

		_fbo.create( Framebuffer::Target::DRAW_FRAMEBUFFER );
		updateOutputFBO( p_renderer );

		_program = VTX_PROGRAM_MANAGER().createProgram( "Selection", { FilePath( "shading/selection.frag" ) } );

		_program->use();

		const Color::Rgba & lineColor = VTX_RENDER_EFFECT().getOutlineColor();
		_program->setVec4f( "uLineColor", lineColor );
	}

	void Selection::resize( const uint p_width, const uint p_height, const GL & p_renderer )
	{
		_texture.resize( p_width, p_height );

		updateOutputFBO( p_renderer );
	}

	void Selection::render( const App::Application::Scene & p_scene, const GL & p_renderer )
	{
		if ( VTX_SETTING().getAA() )
		{
			_fbo.bind();
		}
		else
		{
			p_renderer.getOutputFramebuffer().bind();
		}

		p_renderer.getPassGeometric().getViewPositionsNormalsCompressedTexture().bindToUnit( 0 );
		if ( VTX_RENDER_EFFECT().isOutlineEnabled() )
		{
			p_renderer.getPassOutline().getTexture().bindToUnit( 1 );
		}
		else
		{
			p_renderer.getPassShading().getTexture().bindToUnit( 1 );
		}
		p_renderer.getPassLinearizeDepth().getTexture().bindToUnit( 2 );

		_program->use();

		if ( VTXApp::get().MASK & VTX_MASK_UNIFORM_UPDATED )
		{
			/// TODO: let the user define the line color.
			const Color::Rgba lineColor = Color::Rgba( 45, 243, 26 );
			_program->setVec4f( "uLineColor", lineColor );
		}

		p_renderer.getQuadVAO().drawArray( VertexArray::DrawMode::TRIANGLE_STRIP, 0, 4 );
	}

	void Selection::updateOutputFBO( const GL & p_renderer )
	{
		if ( VTX_SETTING().getAA() )
		{
			_fbo.attachTexture( _texture, Framebuffer::Attachment::COLOR0 );
		}
	}
} // namespace VTX::Renderer::GL::Pass
