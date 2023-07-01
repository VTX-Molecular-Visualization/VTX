#include "renderer/gl/pass/ssao.hpp"

namespace VTX::Renderer::GL::Pass
{
	const float SSAO::AO_INTENSITY_DEFAULT = 5.f;
	const float SSAO::AO_INTENSITY_MIN	   = 1.f;
	const float SSAO::AO_INTENSITY_MAX	   = 20.f;

	SSAO::SSAO( const size_t p_width, const size_t p_height, ProgramManager & p_pm )
	{
		out.fbo		= std::make_unique<Framebuffer>();
		out.texture = std::make_unique<Texture2D>(
			p_width, p_height, GL_R8, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		out.fbo->attachTexture( *out.texture, GL_COLOR_ATTACHMENT0 );

		_ubo = std::make_unique<Buffer>( uniforms, GL_STATIC_DRAW );

		_program = p_pm.createProgram( "SSAO", std::vector<FilePath> { "default.vert", "ssao.frag" } );
		assert( _program != nullptr );

		refreshKernel();
	}

	void SSAO::resize( const size_t p_width, const size_t p_height )
	{
		out.texture->resize( p_width, p_height );
		out.fbo->attachTexture( *out.texture, GL_COLOR_ATTACHMENT0 );
	}

	void SSAO::render( VertexArray & p_vao )
	{
		assert( in.textureDataPacked != nullptr );
		assert( in.textureDepth != nullptr );
		assert( _noiseTexture != nullptr );

		out.fbo->bind( GL_DRAW_FRAMEBUFFER );
		in.textureDataPacked->bindToUnit( 0 );
		_noiseTexture->bindToUnit( 1 );
		in.textureDepth->bindToUnit( 2 );
		_ubo->bind( GL_UNIFORM_BUFFER, 3 );
		_program->use();
		p_vao.drawArray( GL_TRIANGLE_STRIP, 0, 4 );
		in.textureDataPacked->unbindFromUnit( 0 );
		_noiseTexture->unbindFromUnit( 1 );
		in.textureDepth->unbindFromUnit( 2 );
		_ubo->unbind( 3 );
		out.fbo->unbind();
	}

	void SSAO::setIntensity( float p_intensity )
	{
		uniforms.intensity = p_intensity;
		_ubo->setSub( p_intensity, offsetof( StructUniforms, intensity ), sizeof( float ) );
	}

	void SSAO::refreshKernel()
	{
		std::vector<Vec3f> aoKernel = std::vector<Vec3f>( uniforms.kernelSize );

		// generate random ao kernel
		for ( uint i = 0; i < uniforms.kernelSize; i++ )
		{
			// sample on unit hemisphere
			Vec3f v = Util::Math::cosineWeightedHemisphere();

			// scale sample within the hemisphere
			v *= Util::Math::randomFloat();
			// accelerating interpolation (distance from center reduces when number of points grow up)
			float scale = float( i ) / float( uniforms.kernelSize );
			scale		= Util::Math::linearInterpolation( 0.01f, 1.f, scale * scale );
			v *= scale;
			aoKernel[ i ] = v;
		}

		// generate noise texture
		std::vector<Vec3f> noise( uniforms.noiseTextureSize * uniforms.noiseTextureSize );

		for ( uint i = 0; i < noise.size(); ++i )
		{
			noise[ i ] = Vec3f( Util::Math::randomFloat() * 2.f - 1.f,
								Util::Math::randomFloat() * 2.f - 1.f,
								0.f ); // Vec3f([-1;1],[-1;1],0)
			noise[ i ] = Util::Math::normalize( noise[ i ] );
		}

		_noiseTexture = std::make_unique<Texture2D>( uniforms.noiseTextureSize,
													 uniforms.noiseTextureSize,
													 GL_RGB16F,
													 GL_REPEAT,
													 GL_REPEAT,
													 GL_NEAREST,
													 GL_NEAREST );

		_noiseTexture->fill( noise.data() );

		_ubo->setSub( uniforms, 0, sizeof( StructUniforms ) );

		_program->use();
		_program->setVec3fArray( "uAoKernel", uniforms.kernelSize, aoKernel.data() );
	}

} // namespace VTX::Renderer::GL::Pass
