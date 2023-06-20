#include "renderer/gl/pass/pixelize.hpp"

namespace VTX::Renderer::GL::Pass
{
	Pixelize::Pixelize( const size_t p_width, const size_t p_height, ProgramManager & p_pm )
	{
		out.fbo		= std::make_unique<Framebuffer>();
		out.texture = std::make_unique<Texture2D>(
			p_width, p_height, GL_RGBA16F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR );

		out.fbo->attachTexture( *out.texture, GL_COLOR_ATTACHMENT0 );

		_program = p_pm.createProgram( "Pixelize", std::vector<FilePath> { "default.vert", "pixelize.frag" } );
		assert( _program != nullptr );
	}

	void Pixelize::resize( const size_t p_width, const size_t p_height )
	{
		out.texture->resize( p_width, p_height );
		out.fbo->attachTexture( *out.texture, GL_COLOR_ATTACHMENT0 );
	}

	void Pixelize::render( VertexArray & p_vao )
	{
		assert( in.textureDataPacked != nullptr );
		assert( in.textureColor != nullptr );

		out.fbo->bind( GL_DRAW_FRAMEBUFFER );
		in.textureDataPacked->bindToUnit( 0 );
		in.textureColor->bindToUnit( 1 );
		_program->use();
		p_vao.drawArray( GL_TRIANGLE_STRIP, 0, 4 );
		out.fbo->unbind();
	}

} // namespace VTX::Renderer::GL::Pass
