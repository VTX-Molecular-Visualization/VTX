#include "ssao.hpp"
#include "renderer/gl/gl.hpp"
#include "setting.hpp"
#include "util/sampler.hpp"
#include <random>

namespace VTX
{
	namespace Renderer
	{
		namespace Pass
		{
			void SSAO::init( GLSL::ProgramManager & p_programManager, const uint p_width, const uint p_height )
			{
				glGenFramebuffers( 1, &_fboSSAO );
				glBindFramebuffer( GL_FRAMEBUFFER, _fboSSAO );

				glGenTextures( 1, &_ssaoTexture );
				glBindTexture( GL_TEXTURE_2D, _ssaoTexture );
				glTexStorage2D( GL_TEXTURE_2D, 1, GL_R16F, p_width, p_height );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ssaoTexture, 0 );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				_ssaoShader = p_programManager.createProgram( "SSAO", { "shading/ssao.frag" } );

				_uProjMatrixLoc	 = glGetUniformLocation( _ssaoShader->getId(), "uProjMatrix" );
				_uAoKernelLoc	 = glGetUniformLocation( _ssaoShader->getId(), "uAoKernel" );
				_uAoRadiusLoc	 = glGetUniformLocation( _ssaoShader->getId(), "uAoRadius" );
				_uKernelSizeLoc	 = glGetUniformLocation( _ssaoShader->getId(), "uKernelSize" );
				_uAoIntensityLoc = glGetUniformLocation( _ssaoShader->getId(), "uAoIntensity" );

				// generate random ao kernel
				std::vector<Vec3f> aoKernel( _kernelSize );

				for ( int i = 0; i < _kernelSize; i++ )
				{
					// sample on unit hemisphere
					Vec3f v = Util::Sampler::cosineWeightedHemisphere( Util::Math::randomFloat(),
																	   Util::Math::randomFloat() );

					// scale sample within the hemisphere
					v *= Util::Math::randomFloat();
					// accelerating interpolation (distance from center reduces when number of points grow up)
					float scale = float( i ) / float( _kernelSize );
					scale		= Util::Math::linearInterpolation( 0.01f, 1.f, scale * scale );
					v *= scale;
					aoKernel[ i ] = v;
				}

				_ssaoShader->use();
				glUniform3fv( _uAoKernelLoc, _kernelSize, (const GLfloat *)aoKernel.data() );
				glUniform1f( _uAoRadiusLoc, Setting::Rendering::aoRadius );
				glUniform1i( _uAoIntensityLoc, Setting::Rendering::aoIntensity );
				glUniform1i( _uKernelSizeLoc, _kernelSize );

				// generate noise texture
				std::vector<Vec3f> noise( _noiseTextureSize * _noiseTextureSize );

				for ( uint i = 0; i < noise.size(); ++i )
				{
					noise[ i ] = Vec3f( Util::Math::randomFloat() * 2.f - 1.f,
										Util::Math::randomFloat() * 2.f - 1.f,
										0.f ); // Vec3f([-1;1],[-1;1],0)
					noise[ i ] = Util::Math::normalize( noise[ i ] );
				}
				glGenTextures( 1, &_noiseTexture );
				glBindTexture( GL_TEXTURE_2D, _noiseTexture );
				glTexImage2D( GL_TEXTURE_2D,
							  0,
							  GL_RGB16F,
							  _noiseTextureSize,
							  _noiseTextureSize,
							  0,
							  GL_RGB,
							  GL_FLOAT,
							  noise.data() );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				// repeat tile over the image
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
			}

			void SSAO::clean()
			{
				glDeleteFramebuffers( 1, &_fboSSAO );
				glDeleteTextures( 1, &_ssaoTexture );
				glDeleteTextures( 1, &_noiseTexture );
			}

			void SSAO::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				glBindFramebuffer( GL_FRAMEBUFFER, _fboSSAO );
				glClear( GL_COLOR_BUFFER_BIT );

				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassGeometric().getColorNormalCompressedTexture() );
				glActiveTexture( GL_TEXTURE1 );
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassGeometric().getCamSpacePositionsTexture() );
				glActiveTexture( GL_TEXTURE2 );
				glBindTexture( GL_TEXTURE_2D, _noiseTexture );

				_ssaoShader->use();

				// TODO don't aoRadius/aoIntensity/PorjMatrix update each frame
				glUniform1f( _uAoRadiusLoc, Setting::Rendering::aoRadius );
				glUniform1i( _uAoIntensityLoc, Setting::Rendering::aoIntensity );

				glUniformMatrix4fv( _uProjMatrixLoc,
									1,
									GL_FALSE,
									Util::Math::value_ptr( ( p_scene.getCamera().getProjectionMatrix() ) ) );

				glBindVertexArray( p_renderer.getQuadVAO() );
				glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
				glBindVertexArray( 0 );

				// TODO Needed ?
				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D, 0 );
				glActiveTexture( GL_TEXTURE1 );
				glBindTexture( GL_TEXTURE_2D, 0 );
				glActiveTexture( GL_TEXTURE2 );
				glBindTexture( GL_TEXTURE_2D, 0 );
				glActiveTexture( GL_TEXTURE3 );
				glBindTexture( GL_TEXTURE_2D, 0 );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			}

		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
