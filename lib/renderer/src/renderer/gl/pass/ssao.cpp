#include "renderer/gl/pass/ssao.hpp"

namespace VTX::Renderer::GL::Pass
{
	void SSAO::init( const size_t p_width, const size_t p_height )
	{
		out.texture.create( p_width, p_height, GL_R8, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		out.fbo.create();
		out.fbo.attachTexture( out.texture, GL_COLOR_ATTACHMENT0 );

		//_program = VTX_PROGRAM_MANAGER().createProgram( "SSAO", { IO::FilePath( "shading/ssao.frag" ) } );

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

		_noiseTexture.create(
			_noiseTextureSize, _noiseTextureSize, GL_RGB16F, GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST );

		_noiseTexture.fill( noise.data() );

		_program->use();
		_program->setVec3fArray( "uAoKernel", _kernelSize, _aoKernel.data() );
		//_program->setInt( "uAoIntensity", VTX_RENDER_EFFECT().getSSAOIntensity() );
		_program->setInt( "uKernelSize", _kernelSize );
		_program->setFloat( "uNoiseSize", float( _noiseTextureSize ) );
	}

	void SSAO::resize( const size_t p_width, const size_t p_height )
	{
		out.texture.resize( p_width, p_height );
		out.fbo.attachTexture( out.texture, GL_COLOR_ATTACHMENT0 );
	}

	void SSAO::render()
	{
		assert( in.textureViewPositionsNormals != nullptr );
		assert( in.textureLinearizeDepth != nullptr );

		out.fbo.bind( GL_DRAW_FRAMEBUFFER );

		in.textureViewPositionsNormals->bindToUnit( 0 );
		_noiseTexture.bindToUnit( 1 );
		in.textureLinearizeDepth->bindToUnit( 2 );

		/*
		_program->use();

		if ( VTXApp::get().MASK & Render::VTX_MASK_CAMERA_UPDATED )
		{
			_program->setMat4f( "uProjMatrix", p_scene.getCamera().getProjectionMatrix() );
		}

		if ( VTXApp::get().MASK & Render::VTX_MASK_UNIFORM_UPDATED )
		{
			_program->setVec3fArray( "uAoKernel", _kernelSize, _aoKernel.data() );
			_program->setInt( "uAoIntensity", VTX_RENDER_EFFECT().getSSAOIntensity() );
			_program->setInt( "uKernelSize", _kernelSize );
			_program->setFloat( "uNoiseSize", float( _noiseTextureSize ) );
		}

		p_renderer.getQuadVAO().drawArray( VertexArray::DrawMode::TRIANGLE_STRIP, 0, 4 );

		*/
	}

} // namespace VTX::Renderer::GL::Pass
