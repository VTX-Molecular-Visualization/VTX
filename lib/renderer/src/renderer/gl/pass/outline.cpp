#include "renderer/gl/pass/outline.hpp"

namespace VTX::Renderer::GL::Pass
{
	void Outline::init( const size_t p_width, const size_t p_height, ProgramManager & p_pm )
	{
		out.texture.create( p_width, p_height, GL_RGBA16F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR );

		out.fbo.create();
		out.fbo.attachTexture( out.texture, GL_COLOR_ATTACHMENT0 );

		_program = p_pm.createProgram( "Outline", { FilePath( "shading/outline.frag" ) } );
		assert( _program != nullptr );
		/*
		_program->use();
		const Color::Rgba & lineColor = VTX_RENDER_EFFECT().getOutlineColor();
		_program->setVec4f( "uLineColor", lineColor );
		_program->setInt( "uThickness", VTX_RENDER_EFFECT().getOutlineThickness() );
		_program->setFloat( "uSeensivity", VTX_RENDER_EFFECT().getOutlineSensivity() );
		*/
	}

	void Outline::resize( const size_t p_width, const size_t p_height )
	{
		out.texture.resize( p_width, p_height );

		out.fbo.attachTexture( out.texture, GL_COLOR_ATTACHMENT0 );
	}

	void Outline::render()
	{
		assert( in.texture != nullptr );
		assert( in.textureLinearizeDepth != nullptr );

		out.fbo.bind( GL_DRAW_FRAMEBUFFER );

		in.texture->bindToUnit( 0 );
		in.textureLinearizeDepth->bindToUnit( 1 );

		/*
		_program->use();

		if ( VTXApp::get().MASK & VTX_MASK_UNIFORM_UPDATED )
		{
			const Color::Rgba & lineColor = VTX_RENDER_EFFECT().getOutlineColor();
			_program->setVec4f( "uLineColor", lineColor );
			_program->setInt( "uThickness", VTX_RENDER_EFFECT().getOutlineThickness() );
			_program->setFloat( "uSensivity", VTX_RENDER_EFFECT().getOutlineSensivity() );
		}

		p_renderer.getQuadVAO().drawArray( VertexArray::DrawMode::TRIANGLE_STRIP, 0, 4 );
		*/

		out.fbo.unbind();
	}
} // namespace VTX::Renderer::GL::Pass
