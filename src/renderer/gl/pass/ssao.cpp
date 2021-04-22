#include "ssao.hpp"
#include "object3d/camera.hpp"
#include "renderer/gl/gl.hpp"
#include "renderer/gl/program_manager.hpp"
#include "util/sampler.hpp"
#include "vtx_app.hpp"
#include <random>

namespace VTX::Renderer::GL::Pass
{
	void SSAO::init( const uint p_width, const uint p_height, const GL & )
	{
		_texture.create( p_width,
						 p_height,
						 Texture2D::InternalFormat::R8,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Wrapping::CLAMP_TO_EDGE,
						 Texture2D::Filter::NEAREST,
						 Texture2D::Filter::NEAREST );

		_fbo.attachTexture( _texture, Framebuffer::Attachment::COLOR0 );

		_program = VTX_PROGRAM_MANAGER().createProgram( "SSAO", { "shading/ssao.frag" } );

		// generate random ao kernel
		std::vector<Vec3f> aoKernel( _kernelSize );

		for ( uint i = 0; i < _kernelSize; i++ )
		{
			// sample on unit hemisphere
			Vec3f v = Util::Sampler::cosineWeightedHemisphere( Util::Math::randomFloat(), Util::Math::randomFloat() );

			// scale sample within the hemisphere
			v *= Util::Math::randomFloat();
			// accelerating interpolation (distance from center reduces when number of points grow up)
			float scale = float( i ) / float( _kernelSize );
			scale		= Util::Math::linearInterpolation( 0.01f, 1.f, scale * scale );
			v *= scale;
			aoKernel[ i ] = v;
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
							  Texture2D::InternalFormat::RGB16F,
							  Texture2D::Wrapping::REPEAT,
							  Texture2D::Wrapping::REPEAT,
							  Texture2D::Filter::NEAREST,
							  Texture2D::Filter::NEAREST );

		_noiseTexture.fill( noise.data() );

		_program->use();
		_program->setVec3fArray( "uAoKernel", _kernelSize, aoKernel.data() );
		_program->setInt( "uAoIntensity", VTX_SETTING().aoIntensity );
		_program->setInt( "uKernelSize", _kernelSize );
		_program->setFloat( "uNoiseSize", float( _noiseTextureSize ) );
	}

	void SSAO::resize( const uint p_width, const uint p_height, const GL & )
	{
		_texture.resize( p_width, p_height );
		_fbo.attachTexture( _texture, Framebuffer::Attachment::COLOR0 );
	}

	void SSAO::render( const Object3D::Scene & p_scene, const GL & p_renderer )
	{
		_fbo.bind();

		gl()->glBindTextureUnit( 0, p_renderer.getPassGeometric().getViewPositionsNormalsCompressedTexture() );
		gl()->glBindTextureUnit( 1, _noiseTexture.getId() );
		gl()->glBindTextureUnit( 2, p_renderer.getPassLinearizeDepth().getTexture() );

		_program->use();

		if ( VTXApp::get().MASK & VTX_MASK_CAMERA_UPDATED )
		{
			_program->setMat4f( "uProjMatrix", p_scene.getCamera().getProjectionMatrix() );
		}

		if ( VTXApp::get().MASK & VTX_MASK_UNIFORM_UPDATED )
		{
			_program->setInt( "uAoIntensity", VTX_SETTING().aoIntensity );
		}

		gl()->glBindVertexArray( p_renderer.getQuadVAO() );
		gl()->glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		gl()->glBindVertexArray( 0 );
	}

} // namespace VTX::Renderer::GL::Pass
