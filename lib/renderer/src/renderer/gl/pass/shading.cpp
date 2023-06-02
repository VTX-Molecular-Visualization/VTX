#include "renderer/gl/pass/shading.hpp"

namespace VTX::Renderer::GL::Pass
{
	void Shading::init( const size_t p_width, const size_t p_height, ProgramManager & p_pm )
	{
		out.texture.create( p_width, p_height, GL_RGBA16F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		out.fbo.create();
		out.fbo.attachTexture( out.texture, GL_COLOR_ATTACHMENT0 );

		_program = p_pm.createProgram( "Shading", std::vector<FilePath> { "default.vert", "shading.frag" } );

		assert( _program != nullptr );
	}

	void Shading::resize( const size_t p_width, const size_t p_height )
	{
		out.texture.resize( p_width, p_height );
		out.fbo.attachTexture( out.texture, GL_COLOR_ATTACHMENT0 );
	}

	void Shading::render( VertexArray & p_vao )
	{
		assert( in.textureViewPositionsNormals != nullptr );
		assert( in.texture != nullptr );
		assert( in.textureBlur != nullptr );

		out.fbo.bind( GL_DRAW_FRAMEBUFFER );
		in.textureViewPositionsNormals->bindToUnit( 0 );
		in.texture->bindToUnit( 1 );
		in.textureBlur->bindToUnit( 2 );
		_program->use();
		p_vao.drawArray( GL_TRIANGLE_STRIP, 0, 4 );
		out.fbo.unbind();
	}
} // namespace VTX::Renderer::GL::Pass
