#include "renderer/gl/pass/selection.hpp"

namespace VTX::Renderer::GL::Pass
{
	void Selection::init( const size_t p_width, const size_t p_height, ProgramManager & p_pm )
	{
		out.texture.create( p_width, p_height, GL_RGBA16F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR );

		out.fbo.create();
		// updateOutputFBO( p_renderer );

		_program = p_pm.createProgram( "Shading", { FilePath( "shading/shading.frag" ) } );
		assert( _program != nullptr );

		//_program->use();
		// const Color::Rgba & lineColor = VTX_RENDER_EFFECT().getOutlineColor();
		//_program->setVec4f( "uLineColor", lineColor );
	}

	void Selection::resize( const size_t p_width, const size_t p_height )
	{
		out.texture.resize( p_width, p_height );

		// updateOutputFBO( p_renderer );
	}

	void Selection::render()
	{
		assert( in.textureViewPositionsNormals != nullptr );
		assert( in.texture != nullptr );
		assert( in.textureLinearizeDepth != nullptr );

		/*
		if ( VTX_SETTING().getAA() )
		{
			_fbo.bind();
		}
		else
		{
			p_renderer.getOutputFramebuffer().bind();
		}
		*/

		in.textureViewPositionsNormals->bindToUnit( 0 );

		/*
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
		*/
	}

	/*
	void Selection::updateOutputFBO( const GL & p_renderer )
	{
		if ( VTX_SETTING().getAA() )
		{
			_fbo.attachTexture( _texture, Framebuffer::Attachment::COLOR0 );
		}
	}
	*/
} // namespace VTX::Renderer::GL::Pass
