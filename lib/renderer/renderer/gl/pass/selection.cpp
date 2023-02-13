#include "selection.hpp"
#include "gl/gl.hpp"
#include "gl/program_manager.hpp"

namespace VTX::Renderer::GL::Pass
{
	void Selection::init( const size_t p_width, const size_t p_height, const GL & p_renderer )
	{
		_texture.create( p_width,
						 p_height,
						 Wrapper::Texture2D::InternalFormat::RGBA16F,
						 Wrapper::Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Wrapper::Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Wrapper::Texture2D::Filter::LINEAR,
						 Wrapper::Texture2D::Filter::LINEAR );

		_fbo.create( Wrapper::Framebuffer::Target::DRAW_FRAMEBUFFER );
		updateOutputFBO( p_renderer );

		_program = VTX_PROGRAM_MANAGER().createProgram( "Selection", { Util::FilePath( "shading/selection.frag" ) } );

		_program->use();

		// const Color::Rgb & lineColor = VTX_RENDER_EFFECT().getOutlineColor();
		//_program->setVec3f( "uLineColor", lineColor );
	}

	void Selection::resize( const size_t p_width, const size_t p_height, const GL & p_renderer )
	{
		_texture.resize( p_width, p_height );

		updateOutputFBO( p_renderer );
	}

	void Selection::render( const GL & p_renderer )
	{
		/*
		if ( VTX_RENDER_EFFECT().getAA() )
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
			const Color::Rgb lineColor = Color::Rgb( 45, 243, 26 );
			_program->setVec3f( "uLineColor", lineColor );
		}

		p_renderer.getQuadVAO().drawArray( VertexArray::DrawMode::TRIANGLE_STRIP, 0, 4 );
		*/
	}

	void Selection::updateOutputFBO( const GL & p_renderer )
	{
		/*
		if ( VTX_RENDER_EFFECT().getAA() )
		{
			_fbo.attachTexture( _texture, Framebuffer::Attachment::COLOR0 );
		}
		*/
	}
} // namespace VTX::Renderer::GL::Pass