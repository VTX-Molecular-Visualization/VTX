#include "app/render/renderer/gl/pass/blur.hpp"
#include "app/application/render_effect/render_effect_preset.hpp"
#include "app/vtx_app.hpp"
#include "app/render/renderer/gl/gl.hpp"
#include "app/render/renderer/gl/program_manager.hpp"

namespace VTX::App::Render::Renderer::GL::Pass
{
	void Blur::init( const uint p_width, const uint p_height, const GL & )
	{
		_textureFirstPass.create( p_width,
								  p_height,
								  Texture2D::InternalFormat::R16F,
								  Texture2D::Wrapping::CLAMP_TO_EDGE,
								  Texture2D::Wrapping::CLAMP_TO_EDGE,
								  Texture2D::Filter::NEAREST,
								  Texture2D::Filter::NEAREST );

		_fboFirstPass.create( Framebuffer::Target::DRAW_FRAMEBUFFER );
		_fboFirstPass.attachTexture( _textureFirstPass, Framebuffer::Attachment::COLOR0 );

		_texture.create( p_width,
						 p_height,
						 Texture2D::InternalFormat::R16F,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Filter::NEAREST,
						 Texture2D::Filter::NEAREST );
		clearTexture();

		_fbo.create( Framebuffer::Target::DRAW_FRAMEBUFFER );
		_fbo.attachTexture( _texture, Framebuffer::Attachment::COLOR0 );

		_program = VTX_PROGRAM_MANAGER().createProgram( "Blur", { FilePath( "shading/bilateral_blur.frag" ) } );

		_program->use();
		_program->setInt( "uBlurSize", VTX_RENDER_EFFECT().getSSAOBlurSize() );
	}

	void Blur::resize( const uint p_width, const uint p_height, const GL & )
	{
		_textureFirstPass.resize( p_width, p_height );
		_texture.resize( p_width, p_height );

		clearTexture();

		_fboFirstPass.attachTexture( _textureFirstPass, Framebuffer::Attachment::COLOR0 );
		_fbo.attachTexture( _texture, Framebuffer::Attachment::COLOR0 );
	}

	void Blur::render( const App::Application::Scene & p_scene, const GL & p_renderer )
	{
		_fboFirstPass.bind();
		p_renderer.getPassSSAO().getTexture().bindToUnit( 0 );
		p_renderer.getPassLinearizeDepth().getTexture().bindToUnit( 1 );

		_program->use();

		if ( App::VTXApp::get().MASK & Render::VTX_MASK_UNIFORM_UPDATED )
		{
			_program->setInt( "uBlurSize", VTX_RENDER_EFFECT().getSSAOBlurSize() );
		}

		_program->setVec2i( "uDirection", 1, 0 );

		p_renderer.getQuadVAO().drawArray( VertexArray::DrawMode::TRIANGLE_STRIP, 0, 4 );

		_fbo.bind();

		_textureFirstPass.bindToUnit( 0 );
		p_renderer.getPassLinearizeDepth().getTexture().bindToUnit( 1 );

		_program->setVec2i( "uDirection", 0, 1 );

		p_renderer.getQuadVAO().drawArray( VertexArray::DrawMode::TRIANGLE_STRIP, 0, 4 );
	}

	void Blur::clearTexture()
	{
		const float value = 1.f;
		_texture.clear( &value, Texture2D::Format::RED, Texture2D::Type::FLOAT );
	}

} // namespace VTX::App::Render::Renderer::GL::Pass
