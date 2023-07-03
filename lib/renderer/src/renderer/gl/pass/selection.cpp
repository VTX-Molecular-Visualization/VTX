#include "renderer/gl/pass/selection.hpp"

namespace VTX::Renderer::GL::Pass
{
	Selection::Selection( const size_t p_width, const size_t p_height, ProgramManager & p_pm )
	{
		out.fbo		= std::make_unique<Framebuffer>();
		out.texture = std::make_unique<Texture2D>(
			p_width, p_height, GL_RGBA16F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR );

		out.fbo->attachTexture( *out.texture, GL_COLOR_ATTACHMENT0 );

		_ubo = std::make_unique<Buffer>( uniforms, GL_STATIC_DRAW );

		_program = p_pm.createProgram( "Selection", std::vector<FilePath> { "default.vert", "selection.frag" } );
		assert( _program != nullptr );
	}

	void Selection::resize( const size_t p_width, const size_t p_height )
	{
		out.texture->resize( p_width, p_height );
		out.fbo->attachTexture( *out.texture, GL_COLOR_ATTACHMENT0 );
	}

	void Selection::render( VertexArray & p_vao )
	{
		assert( in.textureDataPacked != nullptr );
		assert( in.textureColor != nullptr );
		assert( in.textureDepth != nullptr );

		out.fbo->bind( GL_DRAW_FRAMEBUFFER );
		in.textureDataPacked->bindToUnit( 0 );
		in.textureColor->bindToUnit( 1 );
		in.textureDepth->bindToUnit( 2 );
		_ubo->bind( GL_UNIFORM_BUFFER, 3 );
		_program->use();
		p_vao.drawArray( GL_TRIANGLE_STRIP, 0, 4 );
		in.textureDataPacked->unbindFromUnit( 0 );
		in.textureColor->unbindFromUnit( 1 );
		in.textureDepth->unbindFromUnit( 2 );
		_ubo->unbind( 3 );
		out.fbo->unbind();
	}

	void Selection::setColor( const Util::Color::Rgba & p_color )
	{
		uniforms.color = p_color;
		_ubo->setSub( p_color, offsetof( StructUniforms, color ), sizeof( Util::Color::Rgba ) );
	}

} // namespace VTX::Renderer::GL::Pass
