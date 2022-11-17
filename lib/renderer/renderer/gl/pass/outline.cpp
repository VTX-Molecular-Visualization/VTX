#include "outline.hpp"
#include "gl/gl.hpp"
#include "gl/program_manager.hpp"

namespace VTX::Renderer::GL::Pass
{
	void Outline::init( const size_t p_width, const size_t p_height, const GL & )
	{
		_texture.create( p_width,
						 p_height,
						 Wrapper::Texture2D::InternalFormat::RGBA16F,
						 Wrapper::Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Wrapper::Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Wrapper::Texture2D::Filter::LINEAR,
						 Wrapper::Texture2D::Filter::LINEAR );

		_fbo.create( Wrapper::Framebuffer::Target::DRAW_FRAMEBUFFER );
		_fbo.attachTexture( _texture, Wrapper::Framebuffer::Attachment::COLOR0 );

		_program = VTX_PROGRAM_MANAGER().createProgram( "Outline", { Util::FilePath( "shading/outline.frag" ) } );

		_program->use();

		// const Color::Rgb & lineColor = VTX_RENDER_EFFECT().getOutlineColor();
		//_program->setVec3f( "uLineColor", lineColor );
		//_program->setInt( "uThickness", VTX_RENDER_EFFECT().getOutlineThickness() );
		//_program->setFloat( "uSeensivity", VTX_RENDER_EFFECT().getOutlineSensivity() );
	}

	void Outline::resize( const size_t p_width, const size_t p_height, const GL & )
	{
		_texture.resize( p_width, p_height );

		_fbo.attachTexture( _texture, Wrapper::Framebuffer::Attachment::COLOR0 );
	}

	void Outline::render( const GL & p_renderer )
	{
		_fbo.bind();

		p_renderer.getPassShading().getTexture().bindToUnit( 0 );
		p_renderer.getPassLinearizeDepth().getTexture().bindToUnit( 1 );

		_program->use();

		/*
		if ( VTXApp::get().MASK & VTX_MASK_UNIFORM_UPDATED )
		{
			const Color::Rgb & lineColor = VTX_RENDER_EFFECT().getOutlineColor();
			_program->setVec3f( "uLineColor", lineColor );
			_program->setInt( "uThickness", VTX_RENDER_EFFECT().getOutlineThickness() );
			_program->setFloat( "uSensivity", VTX_RENDER_EFFECT().getOutlineSensivity() );
		}

		p_renderer.getQuadVAO().drawArray( Wrapper::VertexArray::DrawMode::TRIANGLE_STRIP, 0, 4 );
		*/
	}
} // namespace VTX::Renderer::GL::Pass
