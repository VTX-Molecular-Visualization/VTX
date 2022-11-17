#include "ssao.hpp"
#include "model/renderer/render_effect_preset.hpp"
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

		_fbo.create( Framebuffer::Target::DRAW_FRAMEBUFFER );
		_fbo.attachTexture( _texture, Framebuffer::Attachment::COLOR0 );

		_program = VTX_PROGRAM_MANAGER().createProgram( "SSAO", { IO::FilePath( "shading/ssao.frag" ) } );

		// generate random ao kernel
		_aoKernel.resize( _kernelSize );

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
							  Texture2D::InternalFormat::RGB16F,
							  Texture2D::Wrapping::REPEAT,
							  Texture2D::Wrapping::REPEAT,
							  Texture2D::Filter::NEAREST,
							  Texture2D::Filter::NEAREST );

		_noiseTexture.fill( noise.data() );

		_program->use();
		_program->setVec3fArray( "uAoKernel", _kernelSize, _aoKernel.data() );
		_program->setInt( "uAoIntensity", VTX_RENDER_EFFECT().getSSAOIntensity() );
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

		p_renderer.getPassGeometric().getViewPositionsNormalsCompressedTexture().bindToUnit( 0 );
		_noiseTexture.bindToUnit( 1 );
		p_renderer.getPassLinearizeDepth().getTexture().bindToUnit( 2 );

		_program->use();

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
	}

} // namespace VTX::Renderer::GL::Pass
