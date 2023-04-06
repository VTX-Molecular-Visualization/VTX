#include "ssao.hpp"
#include "define.hpp"
#include "gl/gl.hpp"
#include "gl/program_manager.hpp"
#include <math.hpp>
#include <random>

namespace VTX::Renderer::GL::Pass
{
	void SSAO::init( const size_t p_width, const size_t p_height, const GL & )
	{
		_texture.create( p_width,
						 p_height,
						 Wrapper::Texture2D::InternalFormat::R8,
						 Wrapper::Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Wrapper::Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Wrapper::Texture2D::Filter::NEAREST,
						 Wrapper::Texture2D::Filter::NEAREST );

		_fbo.create( Wrapper::Framebuffer::Target::DRAW_FRAMEBUFFER );
		_fbo.attachTexture( _texture, Wrapper::Framebuffer::Attachment::COLOR0 );

		_program = VTX_PROGRAM_MANAGER().createProgram( "SSAO", { VTX::Util::FilePath( "shading/ssao.frag" ) } );

		// generate random ao kernel
		_aoKernel.resize( _kernelSize );

		for ( uint i = 0; i < _kernelSize; i++ )
		{
			// sample on unit hemisphere
			Vec3f v = Util::Math::cosineWeightedHemisphere( Util::Math::randomFloat(), Util::Math::randomFloat() );

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

		_noiseTexture.create( _noiseTextureSize,
							  _noiseTextureSize,
							  Wrapper::Texture2D::InternalFormat::RGB16F,
							  Wrapper::Texture2D::Wrapping::REPEAT,
							  Wrapper::Texture2D::Wrapping::REPEAT,
							  Wrapper::Texture2D::Filter::NEAREST,
							  Wrapper::Texture2D::Filter::NEAREST );

		_noiseTexture.fill( noise.data() );

		_program->use();
		_program->setVec3fArray( "uAoKernel", _kernelSize, _aoKernel.data() );
		//_program->setInt( "uAoIntensity", VTX_RENDER_EFFECT().getSSAOIntensity() );
		_program->setInt( "uKernelSize", _kernelSize );
		_program->setFloat( "uNoiseSize", float( _noiseTextureSize ) );
	}

	void SSAO::resize( const size_t p_width, const size_t p_height, const GL & )
	{
		_texture.resize( p_width, p_height );
		_fbo.attachTexture( _texture, Wrapper::Framebuffer::Attachment::COLOR0 );
	}

	void SSAO::render( const GL & p_renderer )
	{
		_fbo.bind();

		p_renderer.getPassGeometric().getViewPositionsNormalsCompressedTexture().bindToUnit( 0 );
		_noiseTexture.bindToUnit( 1 );
		p_renderer.getPassLinearizeDepth().getTexture().bindToUnit( 2 );

		_program->use();

		/*
		if ( VTXApp::get().MASK & VTX_MASK_CAMERA_UPDATED )
		{
			_program->setMat4f( "uProjMatrix", p_scene.getCamera().getProjectionMatrix() );
		}

		if ( VTXApp::get().MASK & VTX_MASK_UNIFORM_UPDATED )
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
