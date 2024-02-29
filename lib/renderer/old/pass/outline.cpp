#include "renderer/gl/pass/outline.hpp"

namespace VTX::Renderer::GL::Pass
{
	Outline::Outline( const size_t p_width, const size_t p_height, ProgramManager & p_pm )
	{
		out.fbo		= std::make_unique<Framebuffer>();
		out.texture = std::make_unique<Texture2D>(
			p_width, p_height, GL_RGBA16F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR );

		out.fbo->attachTexture( *out.texture, GL_COLOR_ATTACHMENT0 );

		_ubo = std::make_unique<Buffer>( uniforms, GL_STATIC_DRAW );

		_program = p_pm.createProgram( "Outline", std::vector<FilePath> { "default.vert", "outline.frag" } );
		assert( _program != nullptr );
	}

	void Outline::resize( const size_t p_width, const size_t p_height )
	{
		out.texture->resize( p_width, p_height );
		out.fbo->attachTexture( *out.texture, GL_COLOR_ATTACHMENT0 );
	}

	void Outline::render( VertexArray & p_vao )
	{
		assert( in.textureColor != nullptr );
		assert( in.textureDepth != nullptr );

		out.fbo->bind( GL_DRAW_FRAMEBUFFER );
		in.textureColor->bindToUnit( 0 );
		in.textureDepth->bindToUnit( 1 );
		_ubo->bind( GL_UNIFORM_BUFFER, 2 );
		_program->use();
		p_vao.drawArray( GL_TRIANGLE_STRIP, 0, 4 );
		in.textureColor->unbindFromUnit( 0 );
		in.textureDepth->unbindFromUnit( 1 );
		_ubo->unbind( 2 );
		out.fbo->unbind();
	}

	void Outline::setSensivity( const float p_sensivity )
	{
		uniforms.sensivity = p_sensivity;
		_ubo->setSub( p_sensivity, offsetof( StructUniforms, sensivity ), sizeof( float ) );
	}

	void Outline::setThickness( const uint p_thickness )
	{
		uniforms.thickness = p_thickness;
		_ubo->setSub( p_thickness, offsetof( StructUniforms, thickness ), sizeof( uint32_t ) );
	}

	void Outline::setColor( const Util::Color::Rgba & p_color )
	{
		uniforms.color = p_color;
		_ubo->setSub( p_color, offsetof( StructUniforms, color ), sizeof( Util::Color::Rgba ) );
	}
} // namespace VTX::Renderer::GL::Pass