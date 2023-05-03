#include "renderer/gl/pass/fxaa.hpp"

namespace VTX::Renderer::GL::Pass
{
	void FXAA::init( const size_t p_width, const size_t p_height )
	{
		out.texture.create( p_width, p_height, GL_RGBA16F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		//_program = VTX_PROGRAM_MANAGER().createProgram( "AA", { IO::FilePath( "shading/fxaa.frag" ) } );
	}

	void FXAA::resize( const size_t p_width, const size_t p_height ) { out.texture.resize( p_width, p_height ); }

	void FXAA::render()
	{
		assert( in.texture != nullptr );

		/*
		p_renderer.getOutputFramebuffer().bind();

		p_renderer.getPassSelection().getTexture().bindToUnit( 0 );

		_program->use();

		p_renderer.getQuadVAO().drawArray( VertexArray::DrawMode::TRIANGLE_STRIP, 0, 4 );
		*/
	}

} // namespace VTX::Renderer::GL::Pass
