#include "ssao.hpp"
#include "renderer/gl/gl.hpp"
#include "util/sampler.hpp"
#include "vtx_app.hpp"
#include <random>

namespace VTX::Renderer::GL::Pass
{
	SSAO::~SSAO()
	{
		gl()->glDeleteFramebuffers( 1, &_fbo );
		gl()->glDeleteTextures( 1, &_texture );
		gl()->glDeleteTextures( 1, &_noiseTexture );
	}

	void SSAO::init( const uint p_width, const uint p_height )
	{
		gl()->glCreateFramebuffers( 1, &_fbo );

		gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_texture );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		gl()->glTextureStorage2D( _texture, 1, GL_R8, p_width, p_height );

		gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _texture, 0 );

		_program = VTX_PROGRAM_MANAGER().createProgram( "SSAO", { "shading/ssao.frag" } );

		_uProjMatrixLoc	 = gl()->glGetUniformLocation( _program->getId(), "uProjMatrix" );
		_uAoKernelLoc	 = gl()->glGetUniformLocation( _program->getId(), "uAoKernel" );
		_uAoIntensityLoc = gl()->glGetUniformLocation( _program->getId(), "uAoIntensity" );
		_uKernelSizeLoc	 = gl()->glGetUniformLocation( _program->getId(), "uKernelSize" );
		_uNoiseSizeLoc	 = gl()->glGetUniformLocation( _program->getId(), "uNoiseSize" );

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

		gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_noiseTexture );
		gl()->glTextureParameteri( _noiseTexture, GL_TEXTURE_WRAP_S, GL_REPEAT );
		gl()->glTextureParameteri( _noiseTexture, GL_TEXTURE_WRAP_T, GL_REPEAT );
		gl()->glTextureParameteri( _noiseTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		gl()->glTextureParameteri( _noiseTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		gl()->glTextureStorage2D( _noiseTexture, 1, GL_RGB16F, _noiseTextureSize, _noiseTextureSize );
		gl()->glTextureSubImage2D(
			_noiseTexture, 0, 0, 0, _noiseTextureSize, _noiseTextureSize, GL_RGB, GL_FLOAT, noise.data() );

		_program->use();
		gl()->glUniform3fv( _uAoKernelLoc, _kernelSize, (const GLfloat *)aoKernel.data() );
		gl()->glUniform1i( _uAoIntensityLoc, VTX_SETTING().aoIntensity );
		gl()->glUniform1i( _uKernelSizeLoc, _kernelSize );
		gl()->glUniform1f( _uNoiseSizeLoc, float( _noiseTextureSize ) );
	}

	void SSAO::resize( const uint p_width, const uint p_height )
	{
		gl()->glDeleteTextures( 1, &_texture );
		gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_texture );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		gl()->glTextureParameteri( _texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		gl()->glTextureStorage2D( _texture, 1, GL_R8, p_width, p_height );

		gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _texture, 0 );
	}

	void SSAO::render( const Object3D::Scene & p_scene, const GL & p_renderer )
	{
		gl()->glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

		gl()->glBindTextureUnit( 0, p_renderer.getPassGeometric().getViewPositionsNormalsCompressedTexture() );
		gl()->glBindTextureUnit( 1, _noiseTexture );
		gl()->glBindTextureUnit( 2, p_renderer.getPassLinearizeDepth().getTexture() );

		_program->use();

		// TODO don't update each frame
		gl()->glUniform1i( _uAoIntensityLoc, VTX_SETTING().aoIntensity );

		gl()->glUniformMatrix4fv(
			_uProjMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( ( p_scene.getCamera().getProjectionMatrix() ) ) );

		gl()->glBindVertexArray( p_renderer.getQuadVAO() );
		gl()->glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
		gl()->glBindVertexArray( 0 );

		gl()->glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

} // namespace VTX::Renderer::GL::Pass
