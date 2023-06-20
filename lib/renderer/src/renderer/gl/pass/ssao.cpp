#include "renderer/gl/pass/ssao.hpp"

namespace VTX::Renderer::GL::Pass
{
	SSAO::SSAO( const size_t p_width, const size_t p_height, ProgramManager & p_pm )
	{
		out.fbo		= std::make_unique<Framebuffer>();
		out.texture = std::make_unique<Texture2D>(
			p_width, p_height, GL_R8, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		out.fbo->attachTexture( *out.texture, GL_COLOR_ATTACHMENT0 );

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

		out.fbo->bind( GL_DRAW_FRAMEBUFFER );
		in.textureDataPacked->bindToUnit( 0 );
		_noiseTexture->bindToUnit( 1 );
		in.textureDepth->bindToUnit( 2 );
		_program->use();
		p_vao.drawArray( GL_TRIANGLE_STRIP, 0, 4 );
		out.fbo->unbind();
	}

	void SSAO::refreshKernel()
	{
		// generate random ao kernel
		_aoKernel.resize( _kernelSize );

		for ( uint i = 0; i < _kernelSize; i++ )
		{
			// sample on unit hemisphere
			Vec3f v = Util::Math::cosineWeightedHemisphere();

			// scale sample within the hemisphere
			v *= Util::Math::randomFloat();
			// accelerating interpolation (distance from center reduces when number of points grow up)
			float scale = float( i ) / float( _kernelSize );
			scale		= Util::Math::linearInterpolation( 0.01f, 1.f, scale * scale );
			v *= scale;
			_aoKernel[ i ] = v;
		}

		// generate noise texture
		std::vector<Vec3f> noise( _noiseTextureSize * _noiseTextureSize );

		for ( uint i = 0; i < noise.size(); ++i )
		{
			noise[ i ] = Vec3f( Util::Math::randomFloat() * 2.f - 1.f,
								Util::Math::randomFloat() * 2.f - 1.f,
								0.f ); // Vec3f([-1;1],[-1;1],0)
			noise[ i ] = Util::Math::normalize( noise[ i ] );
		}

		_noiseTexture = std::make_unique<Texture2D>(
			_noiseTextureSize, _noiseTextureSize, GL_RGB16F, GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST );

		_noiseTexture->fill( noise.data() );

		_program->use();
		_program->setVec3fArray( "uAoKernel", _kernelSize, _aoKernel.data() );
		_program->setInt( "uKernelSize", _kernelSize );
		_program->setFloat( "uNoiseSize", float( _noiseTextureSize ) );
	}

} // namespace VTX::Renderer::GL::Pass
