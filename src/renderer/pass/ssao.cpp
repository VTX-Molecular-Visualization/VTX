#include "ssao.hpp"
#include "renderer/gl.hpp"
#include "setting.hpp"
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
				glTexImage2D( GL_TEXTURE_2D, 0, GL_R8, p_width, p_height, 0, GL_RED, GL_FLOAT, NULL );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ssaoTexture, 0 );
				static const GLenum drawBufferSSAO[] = { GL_COLOR_ATTACHMENT0 };
				glDrawBuffers( 1, drawBufferSSAO );

				_ssaoShader = p_programManager.createProgram( "SSAO", { "shading/ssao.frag" } );

				_uProjMatrixLoc	 = glGetUniformLocation( _ssaoShader->getId(), "uProjMatrix" );
				_uAoKernelLoc	 = glGetUniformLocation( _ssaoShader->getId(), "uAoKernel" );
				_uAoRadiusLoc	 = glGetUniformLocation( _ssaoShader->getId(), "uAoRadius" );
				_uKernelSizeLoc	 = glGetUniformLocation( _ssaoShader->getId(), "uKernelSize" );
				_uAoIntensityLoc = glGetUniformLocation( _ssaoShader->getId(), "uAoIntensity" );

				// generate random ao kernel
				std::random_device							 rd;
				static std::mt19937							 gen( 0 ); // rd());
				static std::uniform_real_distribution<float> dist( 0.f, 1.f );
				std::vector<Vec3f>							 aoKernel( _kernelSize );

				// for ( int i = 0; i < _kernelSize; i++ )
				int i = 0;
				while ( i < _kernelSize )
				{
					// sample on unit hemisphere
					Vec3f v(
						dist( gen ) * 2.f - 1.f, dist( gen ) * 2.f - 1.f, dist( gen ) ); // Vec3f([-1;1],[-1;1],[0;1])
					v = Util::Math::normalize( v );
					if ( Util::Math::dot( VEC3F_Z, v ) < 0.15 ) continue;
					// scale sample within the hemisphere
					v *= dist( gen );
					// accelerating interpolation (distance from center reduces when number of points grow up)
					float scale = float( i ) / float( _kernelSize );
					scale		= Util::Math::linearInterpolation( 0.1f, 1.f, scale * scale );
					v *= scale;
					aoKernel[ i ] = v;
					i++;
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
					noise[ i ]
						= Vec3f( dist( gen ) * 2.f - 1.f, dist( gen ) * 2.f - 1.f, 0.f ); // Vec3f([-1;1],[-1;1],0)
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

				if ( Setting::Rendering::useSSAO == false ) return;

				glActiveTexture( GL_TEXTURE0 );
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassGeometric().getColorNormalCompressedTexture() );
				glActiveTexture( GL_TEXTURE1 );
				glBindTexture( GL_TEXTURE_2D, p_renderer.getPassGeometric().getCamSpacePositionsTexture() );
				glActiveTexture( GL_TEXTURE2 );
				glBindTexture( GL_TEXTURE_2D, _noiseTexture );

				_ssaoShader->use();

				// TODO don't aoRadius update each frame
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
			}

		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
