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
				glDeleteFramebuffers( 1, &_fbo );
				glDeleteTextures( 1, &_texture );
				glDeleteTextures( 1, &_noiseTexture );
			}

			void SSAO::init( GLSL::ProgramManager & p_programManager, const uint p_width, const uint p_height )
			{
				glGenFramebuffers( 1, &_fbo );
				glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				glGenTextures( 1, &_texture );
				glBindTexture( GL_TEXTURE_2D, _texture );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_R8, p_width, p_height, 0, GL_RED, GL_FLOAT, nullptr );

				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0 );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				_program = p_programManager.createProgram( "SSAO", { "shading/ssao.frag" } );

				_uProjMatrixLoc	 = glGetUniformLocation( _program->getId(), "uProjMatrix" );
				_uAoKernelLoc	 = glGetUniformLocation( _program->getId(), "uAoKernel" );
				_uAoIntensityLoc = glGetUniformLocation( _program->getId(), "uAoIntensity" );
				_uKernelSizeLoc	 = glGetUniformLocation( _program->getId(), "uKernelSize" );
				_uNoiseSizeLoc	 = glGetUniformLocation( _program->getId(), "uNoiseSize" );

				// generate random ao kernel
				std::vector<Vec3f> aoKernel( _kernelSize );

				for ( uint i = 0; i < _kernelSize; i++ )
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
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				// repeat tile over the image
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
				glTexImage2D( GL_TEXTURE_2D,
							  0,
							  GL_RGB16F,
							  _noiseTextureSize,
							  _noiseTextureSize,
							  0,
							  GL_RGB,
							  GL_FLOAT,
							  noise.data() );

				_program->use();
				glUniform3fv( _uAoKernelLoc, _kernelSize, (const GLfloat *)aoKernel.data() );
				glUniform1i( _uAoIntensityLoc, VTX_SETTING().aoIntensity );
				glUniform1i( _uKernelSizeLoc, _kernelSize );
				glUniform1f( _uNoiseSizeLoc, float( _noiseTextureSize ) );
			}

			void SSAO::resize( const uint p_width, const uint p_height )
			{
				glBindTexture( GL_TEXTURE_2D, _texture );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_R8, p_width, p_height, 0, GL_RED, GL_FLOAT, nullptr );
			}

			void SSAO::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D,
							   p_renderer.getPassGeometric().getViewPositionsNormalsCompressedTexture() );
				glActiveTexture( GL_TEXTURE1 );
				glBindTexture( GL_TEXTURE_2D, _noiseTexture );
				glActiveTexture( GL_TEXTURE2 );
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassLinearizeDepth().getTexture() );

				_program->use();

				// TODO don't update each frame
				glUniform1i( _uAoIntensityLoc, VTX_SETTING().aoIntensity );

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

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			}

		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
