#include "renderer/gl/pass/linearize_depth.hpp"

namespace VTX::Renderer::GL::Pass
{
	void LinearizeDepth::init( const size_t p_width, const size_t p_height, ProgramManager & p_pm )
	{
		out.texture.create( p_width, p_height, GL_R32F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		out.fbo.create();
		out.fbo.attachTexture( out.texture, GL_COLOR_ATTACHMENT0 );

		_program
			= p_pm.createProgram( "LinearizeDepth", std::vector<FilePath> { "default.vert", "linearize_depth.frag" } );
		assert( _program != nullptr );
	}

	void LinearizeDepth::resize( const size_t p_width, const size_t p_height )
	{
		out.texture.resize( p_width, p_height );

		out.fbo.attachTexture( out.texture, GL_COLOR_ATTACHMENT0 );
	}

	void LinearizeDepth::render( VertexArray & p_vao )
	{
		assert( in.textureDepth != nullptr );

		out.fbo.bind( GL_DRAW_FRAMEBUFFER );
		in.textureDepth->bind( 0 );
		_program->use();
		p_vao.drawArray( GL_TRIANGLE_STRIP, 0, 4 );
		out.fbo.unbind();
	}
} // namespace VTX::Renderer::GL::Pass
