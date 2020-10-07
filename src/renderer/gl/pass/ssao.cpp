#include "ssao.hpp"
#include "renderer/gl/gl.hpp"
#include "util/sampler.hpp"
#include "vtx_app.hpp"
#include <random>

namespace VTX
{
	namespace Renderer
	{
		namespace Pass
		{
			SSAO::~SSAO()
			{
				OGL().glDeleteFramebuffers( 1, &_fbo );
				OGL().glDeleteTextures( 1, &_texture );
				OGL().glDeleteTextures( 1, &_noiseTexture );
			}

			void SSAO::init( GLSL::ProgramManager & p_programManager, const uint p_width, const uint p_height )
			{
				OGL().glGenFramebuffers( 1, &_fbo );
				OGL().glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				OGL().glGenTextures( 1, &_texture );
				OGL().glBindTexture( GL_TEXTURE_2D, _texture );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				OGL().glTexImage2D( GL_TEXTURE_2D, 0, GL_R8, p_width, p_height, 0, GL_RED, GL_FLOAT, nullptr );

				OGL().glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0 );

				OGL().glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				_program = p_programManager.createProgram( "SSAO", { "shading/ssao.frag" } );

				_uProjMatrixLoc	 = OGL().glGetUniformLocation( _program->getId(), "uProjMatrix" );
				_uAoKernelLoc	 = OGL().glGetUniformLocation( _program->getId(), "uAoKernel" );
				_uAoIntensityLoc = OGL().glGetUniformLocation( _program->getId(), "uAoIntensity" );
				_uKernelSizeLoc	 = OGL().glGetUniformLocation( _program->getId(), "uKernelSize" );
				_uNoiseSizeLoc	 = OGL().glGetUniformLocation( _program->getId(), "uNoiseSize" );

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
					noise[ i ] = Vec3f( Util::Math::randomFloat() * 2.f - 1.f, Util::Math::randomFloat() * 2.f - 1.f,
										0.f ); // Vec3f([-1;1],[-1;1],0)
					noise[ i ] = Util::Math::normalize( noise[ i ] );
				}
				OGL().glGenTextures( 1, &_noiseTexture );
				OGL().glBindTexture( GL_TEXTURE_2D, _noiseTexture );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				// repeat tile over the image
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
				OGL().glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
				OGL().glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB16F, _noiseTextureSize, _noiseTextureSize, 0, GL_RGB, GL_FLOAT, noise.data() );

				_program->use();
				OGL().glUniform3fv( _uAoKernelLoc, _kernelSize, (const GLfloat *)aoKernel.data() );
				OGL().glUniform1i( _uAoIntensityLoc, VTX_SETTING().aoIntensity );
				OGL().glUniform1i( _uKernelSizeLoc, _kernelSize );
				OGL().glUniform1f( _uNoiseSizeLoc, float( _noiseTextureSize ) );
			}

			void SSAO::resize( const uint p_width, const uint p_height )
			{
				OGL().glBindTexture( GL_TEXTURE_2D, _texture );
				OGL().glTexImage2D( GL_TEXTURE_2D, 0, GL_R8, p_width, p_height, 0, GL_RED, GL_FLOAT, nullptr );
			}

			void SSAO::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				OGL().glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				OGL().glActiveTexture( GL_TEXTURE0 );
				OGL().glBindTexture( GL_TEXTURE_2D, p_renderer.getPassGeometric().getViewPositionsNormalsCompressedTexture() );
				OGL().glActiveTexture( GL_TEXTURE1 );
				OGL().glBindTexture( GL_TEXTURE_2D, _noiseTexture );
				OGL().glActiveTexture( GL_TEXTURE2 );
				OGL().glBindTexture( GL_TEXTURE_2D, p_renderer.getPassLinearizeDepth().getTexture() );

				_program->use();

				// TODO don't update each frame
				OGL().glUniform1i( _uAoIntensityLoc, VTX_SETTING().aoIntensity );

				OGL().glUniformMatrix4fv( _uProjMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( ( p_scene.getCamera().getProjectionMatrix() ) ) );

				OGL().glBindVertexArray( p_renderer.getQuadVAO() );
				OGL().glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
				OGL().glBindVertexArray( 0 );

				// TODO Needed ?
				OGL().glActiveTexture( GL_TEXTURE0 );
				OGL().glBindTexture( GL_TEXTURE_2D, 0 );
				OGL().glActiveTexture( GL_TEXTURE1 );
				OGL().glBindTexture( GL_TEXTURE_2D, 0 );
				OGL().glActiveTexture( GL_TEXTURE2 );
				OGL().glBindTexture( GL_TEXTURE_2D, 0 );

				OGL().glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			}

		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
