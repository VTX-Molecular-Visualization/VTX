#include "renderer/gl/pass/shading.hpp"

namespace VTX::Renderer::GL::Pass
{

	Shading::Shading( const size_t p_width, const size_t p_height, ProgramManager & p_pm )
	{
		out.fbo		= std::make_unique<Framebuffer>();
		out.texture = std::make_unique<Texture2D>(
			p_width, p_height, GL_RGBA16F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		out.fbo->attachTexture( *out.texture, GL_COLOR_ATTACHMENT0 );

		_ubo = std::make_unique<Buffer>( uniforms, GL_STATIC_DRAW );

		_program = p_pm.createProgram( "Shading", std::vector<FilePath> { "default.vert", "shading.frag" } );

		assert( _program != nullptr );
	}

	void Shading::resize( const size_t p_width, const size_t p_height )
	{
		out.texture->resize( p_width, p_height );
		out.fbo->attachTexture( *out.texture, GL_COLOR_ATTACHMENT0 );
	}

	void Shading::render( VertexArray & p_vao )
	{
		assert( in.textureDataPacked != nullptr );
		assert( in.textureColor != nullptr );
		assert( in.textureBlur != nullptr );

		out.fbo->bind( GL_DRAW_FRAMEBUFFER );
		in.textureDataPacked->bindToUnit( 0 );
		in.textureColor->bindToUnit( 1 );
		in.textureBlur->bindToUnit( 2 );
		_ubo->bind( GL_UNIFORM_BUFFER, 3 );
		_program->use();
		p_vao.drawArray( GL_TRIANGLE_STRIP, 0, 4 );
		in.textureDataPacked->unbindFromUnit( 0 );
		in.textureColor->unbindFromUnit( 1 );
		in.textureBlur->unbindFromUnit( 2 );
		_ubo->unbind( 3 );
		out.fbo->unbind();
	}

	void Shading::setMode( const ENUM_SHADING p_shading )
	{
		uniforms.shadingMode = p_shading;
		_ubo->setSub( p_shading, offsetof( StructUniforms, shadingMode ), sizeof( ENUM_SHADING ) );
	}

	void Shading::setSpecularFactor( const float p_factor )
	{
		uniforms.specularFactor = p_factor;
		_ubo->setSub( p_factor, offsetof( StructUniforms, specularFactor ), sizeof( float ) );
	}

	void Shading::setToonSteps( uint p_steps )
	{
		uniforms.toonSteps = p_steps;
		_ubo->setSub( p_steps, offsetof( StructUniforms, toonSteps ), sizeof( uint ) );
	}

	void Shading::setColorBackground( const Util::Color::Rgba & p_color )
	{
		uniforms.colorBackground = p_color;
		_ubo->setSub( p_color, offsetof( StructUniforms, colorBackground ), sizeof( Util::Color::Rgba ) );
	}

	void Shading::setColorLight( const Util::Color::Rgba & p_color )
	{
		uniforms.colorLight = p_color;
		_ubo->setSub( p_color, offsetof( StructUniforms, colorLight ), sizeof( Util::Color::Rgba ) );
	}

	void Shading::setColorFog( const Util::Color::Rgba & p_color )
	{
		uniforms.colorFog = p_color;
		_ubo->setSub( p_color, offsetof( StructUniforms, colorFog ), sizeof( Util::Color::Rgba ) );
	}

	void Shading::setFogNear( const float p_near )
	{
		uniforms.fogNear = p_near;
		_ubo->setSub( p_near, offsetof( StructUniforms, fogNear ), sizeof( float ) );
	}

	void Shading::setFogFar( const float p_far )
	{
		uniforms.fogFar = p_far;
		_ubo->setSub( p_far, offsetof( StructUniforms, fogFar ), sizeof( float ) );
	}

	void Shading::setFogDensity( const float p_density )
	{
		uniforms.fogDensity = p_density;
		_ubo->setSub( p_density, offsetof( StructUniforms, fogDensity ), sizeof( float ) );
	}

} // namespace VTX::Renderer::GL::Pass
