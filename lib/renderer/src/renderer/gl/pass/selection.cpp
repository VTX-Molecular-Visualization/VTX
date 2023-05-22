#include "renderer/gl/pass/selection.hpp"

namespace VTX::Renderer::GL::Pass
{
	Selection::Selection( const size_t p_width, const size_t p_height, ProgramManager & p_pm ) :
		out( { Framebuffer(),
			   Texture2D( p_width, p_height, GL_RGBA16F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR ) } )
	{
		out.fbo.attachTexture( out.texture, GL_COLOR_ATTACHMENT0 );

		_program = p_pm.createProgram( "Shading", std::vector<FilePath> { "default.vert", "shading.frag" } );
		assert( _program != nullptr );
	}

	void Selection::resize( const size_t p_width, const size_t p_height ) { out.texture.resize( p_width, p_height ); }

	void Selection::render( VertexArray & p_vao )
	{
		assert( in.textureViewPositionsNormals != nullptr );
		assert( in.texture != nullptr );
		assert( in.textureDepth != nullptr );

		out.fbo.bind( GL_DRAW_FRAMEBUFFER );
		in.textureViewPositionsNormals->bind( 0 );
		in.texture->bind( 1 );
		in.textureDepth->bind( 2 );
		_program->use();
		p_vao.drawArray( GL_TRIANGLE_STRIP, 0, 4 );
		out.fbo.unbind();
	}

} // namespace VTX::Renderer::GL::Pass
