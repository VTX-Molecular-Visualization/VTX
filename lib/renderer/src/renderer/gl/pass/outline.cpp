#include "renderer/gl/pass/outline.hpp"

namespace VTX::Renderer::GL::Pass
{
	void Outline::init( const size_t p_width, const size_t p_height )
	{
		_texture.create( p_width, p_height, GL_RGBA16F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR );

		_fbo.create();
		_fbo.attachTexture( _texture, GL_COLOR_ATTACHMENT0 );

		//_program = VTX_PROGRAM_MANAGER().createProgram( "Outline", { IO::FilePath( "shading/outline.frag" ) } );

		_program->use();

		/*
		const Color::Rgba & lineColor = VTX_RENDER_EFFECT().getOutlineColor();
		_program->setVec4f( "uLineColor", lineColor );
		_program->setInt( "uThickness", VTX_RENDER_EFFECT().getOutlineThickness() );
		_program->setFloat( "uSeensivity", VTX_RENDER_EFFECT().getOutlineSensivity() );
		*/
	}

	void Outline::resize( const size_t p_width, const size_t p_height )
	{
		_texture.resize( p_width, p_height );

		_fbo.attachTexture( _texture, GL_COLOR_ATTACHMENT0 );
	}

	void Outline::render()
	{
		_fbo.bind( GL_DRAW_FRAMEBUFFER );

		/*
		p_renderer.getPassShading().getTexture().bindToUnit( 0 );
		p_renderer.getPassLinearizeDepth().getTexture().bindToUnit( 1 );

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
	}
} // namespace VTX::Renderer::GL::Pass
