#include "gl.hpp"
#include "glm/gtx/compatibility.hpp"
#include "model/molecule.hpp"
#include "setting.hpp"
#include "view/base_view_3d_molecule.hpp"
#include "vtx_app.hpp"
#include <random>

namespace VTX
{
	namespace Renderer
	{
		GL::~GL()
		{
			glDeleteTextures( 1, &_colorNormalCompressedTexture );
			glDeleteTextures( 1, &_camSpacePositionsTexture );
			glDeleteTextures( 1, &_depthTexture );
			glDeleteTextures( 1, &_ssaoTexture );
			glDeleteTextures( 1, &_noiseTexture );
			glDeleteTextures( 1, &_blurTexture );
			glDeleteTextures( 1, &_shadingTexture );
		}

		void GL::init( const Object3D::Scene & p_scene, const uint p_width, const uint p_height )
		{
			if ( _isInitialized ) { return; }
			VTX_INFO( "Initializing renderer..." );

			_width	= p_width;
			_height = p_height;

			// Clear.
			clear( p_scene );

			// Init pass.
			_initGeometricPass();
			_initSsaoPass();
			_initBlurPass();
			_initShadingPass();
			//_initAntiAliasingPass();
			_initQuadVAO();

			_isInitialized = true;
			VTX_INFO( "Renderer initialized" );
		}

		void GL::resize( const uint p_width, const uint p_height )
		{
			if ( p_width != _width || p_height != _height )
			{
				BaseRenderer::resize( p_width, p_height );

				glDeleteTextures( 1, &_colorNormalCompressedTexture );
				glDeleteTextures( 1, &_camSpacePositionsTexture );
				glDeleteTextures( 1, &_depthTexture );
				glDeleteTextures( 1, &_ssaoTexture );
				glDeleteTextures( 1, &_noiseTexture );
				glDeleteTextures( 1, &_blurTexture );
				glDeleteTextures( 1, &_shadingTexture );

				// Geometric pass.
				glBindFramebuffer( GL_FRAMEBUFFER, _fboGeo );

				glGenTextures( 1, &_colorNormalCompressedTexture );
				glBindTexture( GL_TEXTURE_2D, _colorNormalCompressedTexture );
				glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGBA32UI, _width, _height );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

				glGenTextures( 1, &_camSpacePositionsTexture );
				glBindTexture( GL_TEXTURE_2D, _camSpacePositionsTexture );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, _width, _height, 0, GL_RGBA, GL_FLOAT, NULL );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

				glGenTextures( 1, &_depthTexture );
				glBindTexture( GL_TEXTURE_2D, _depthTexture );
				glTexStorage2D( GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, _width, _height );

				glFramebufferTexture2D(
					GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorNormalCompressedTexture, 0 );
				glFramebufferTexture2D(
					GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _camSpacePositionsTexture, 0 );
				glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthTexture, 0 );

				static const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

				glDrawBuffers( 2, drawBuffers );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				GLenum fboStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );
				if ( fboStatus != GL_FRAMEBUFFER_COMPLETE ) { VTX_WARNING( "Framebuffer not complete: " + fboStatus ); }

				GLenum glstatus = glGetError();
				if ( glstatus != GL_NO_ERROR ) { VTX_ERROR( "Error in GL call: " + glstatus ); }

				// SSAO pass.
				glBindFramebuffer( GL_FRAMEBUFFER, _fboSSAO );

				glGenTextures( 1, &_ssaoTexture );
				glBindTexture( GL_TEXTURE_2D, _ssaoTexture );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, _width, _height, 0, GL_RGB, GL_FLOAT, NULL );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ssaoTexture, 0 );
				static const GLenum drawBufferSSAO[] = { GL_COLOR_ATTACHMENT0 };
				glDrawBuffers( 1, drawBufferSSAO );

				// Blur pass.
				glBindFramebuffer( GL_FRAMEBUFFER, _fboBlur );

				glGenTextures( 1, &_blurTexture );
				glBindTexture( GL_TEXTURE_2D, _blurTexture );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, _width, _height, 0, GL_RGB, GL_FLOAT, NULL );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _blurTexture, 0 );
				static const GLenum drawBufferBlur[] = { GL_COLOR_ATTACHMENT0 };
				glDrawBuffers( 1, drawBufferBlur );

				// Shading pass.
				glBindFramebuffer( GL_FRAMEBUFFER, _fboShading );

				glGenTextures( 1, &_shadingTexture );
				glBindTexture( GL_TEXTURE_2D, _shadingTexture );
				glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGBA32F, _width, _height );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

				glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _shadingTexture, 0 );

				static const GLenum draw_bufferShading[] = { GL_COLOR_ATTACHMENT0 };
				glDrawBuffers( 1, draw_bufferShading );
			}
		}

		void GL::_initGeometricPass()
		{
			// TODO: Only when using point sprites.
			glEnable( GL_PROGRAM_POINT_SIZE );
			glPointParameteri( GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT );
			glViewport( 0, 0, _width, _height );

			// Create G-buffers for deferred shading.
			glGenFramebuffers( 1, &_fboGeo );

			glBindFramebuffer( GL_FRAMEBUFFER, _fboGeo );

			glGenTextures( 1, &_colorNormalCompressedTexture );
			glBindTexture( GL_TEXTURE_2D, _colorNormalCompressedTexture );
			glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGBA32UI, _width, _height );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

			glGenTextures( 1, &_camSpacePositionsTexture );
			glBindTexture( GL_TEXTURE_2D, _camSpacePositionsTexture );
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, _width, _height, 0, GL_RGBA, GL_FLOAT, NULL );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

			glGenTextures( 1, &_depthTexture );
			glBindTexture( GL_TEXTURE_2D, _depthTexture );
			glTexStorage2D( GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, _width, _height );

			glFramebufferTexture2D(
				GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorNormalCompressedTexture, 0 );
			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _camSpacePositionsTexture, 0 );
			glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthTexture, 0 );

			static const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

			glDrawBuffers( 2, drawBuffers );

			glBindFramebuffer( GL_FRAMEBUFFER, 0 );

			GLenum fboStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );
			if ( fboStatus != GL_FRAMEBUFFER_COMPLETE ) { VTX_WARNING( "Framebuffer not complete: " + fboStatus ); }

			GLenum glstatus = glGetError();
			if ( glstatus != GL_NO_ERROR ) { VTX_ERROR( "Error in GL call: " + glstatus ); }
		}

		void GL::_initSsaoPass()
		{
			glGenFramebuffers( 1, &_fboSSAO );
			glBindFramebuffer( GL_FRAMEBUFFER, _fboSSAO );

			glGenTextures( 1, &_ssaoTexture );
			glBindTexture( GL_TEXTURE_2D, _ssaoTexture );
			glTexImage2D( GL_TEXTURE_2D, 0, GL_R8, _width, _height, 0, GL_RGB, GL_FLOAT, NULL );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ssaoTexture, 0 );
			static const GLenum drawBufferSSAO[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers( 1, drawBufferSSAO );

			_ssaoShader = VTXApp::get().getProgramManager().createProgram( "SSAO", { "shading/ssao.frag" } );

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
				Vec3f v( dist( gen ) * 2.f - 1.f, dist( gen ) * 2.f - 1.f, dist( gen ) ); // Vec3f([-1;1],[-1;1],[0;1])
				v = glm::normalize( v );
				if ( glm::dot( VEC3F_Z, v ) < 0.15 ) continue;
				// scale sample within the hemisphere
				v *= dist( gen );
				// accelerating interpolation (distance from center reduces when number of points grow up)
				float scale = float( i ) / float( _kernelSize );
				scale		= glm::lerp( 0.1f, 1.f, scale * scale );
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
				noise[ i ] = Vec3f( dist( gen ) * 2.f - 1.f, dist( gen ) * 2.f - 1.f, 0.f ); // Vec3f([-1;1],[-1;1],0)
				noise[ i ] = glm::normalize( noise[ i ] );
			}
			glGenTextures( 1, &_noiseTexture );
			glBindTexture( GL_TEXTURE_2D, _noiseTexture );
			glTexImage2D(
				GL_TEXTURE_2D, 0, GL_RGB16F, _noiseTextureSize, _noiseTextureSize, 0, GL_RGB, GL_FLOAT, noise.data() );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			// repeat tile over the image
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		}

		void GL::_initBlurPass()
		{
			glGenFramebuffers( 1, &_fboBlur );
			glBindFramebuffer( GL_FRAMEBUFFER, _fboBlur );

			glGenTextures( 1, &_blurTexture );
			glBindTexture( GL_TEXTURE_2D, _blurTexture );
			glTexImage2D( GL_TEXTURE_2D, 0, GL_R8, _width, _height, 0, GL_RGB, GL_FLOAT, NULL );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _blurTexture, 0 );
			static const GLenum drawBufferBlur[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers( 1, drawBufferBlur );

			_blurShader = VTXApp::get().getProgramManager().createProgram( "Blur", { "shading/blur.frag" } );

			_uBlurSizeLoc = glGetUniformLocation( _blurShader->getId(), "uBlurSize" );

			_blurShader->use();
			glUniform1i( _uBlurSizeLoc, Setting::Rendering::aoBlurSize );
		}

		void GL::_initShadingPass()
		{
			GLSL::ProgramManager & programManager = VTXApp::get().getProgramManager();

			glGenFramebuffers( 1, &_fboShading );
			glBindFramebuffer( GL_FRAMEBUFFER, _fboShading );

			glGenTextures( 1, &_shadingTexture );
			glBindTexture( GL_TEXTURE_2D, _shadingTexture );
			glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGBA32F, _width, _height );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

			glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _shadingTexture, 0 );

			static const GLenum draw_bufferShading[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers( 1, draw_bufferShading );

			_toonShading	= programManager.createProgram( "ToonShading", { "shading/toonShading.frag" } );
			_diffuseShading = programManager.createProgram( "DiffuseShading", { "shading/diffuseShading.frag" } );
			_blinnPhongShading
				= programManager.createProgram( "BlinnPhongShading", { "shading/blinnPhongShading.frag" } );

			// Use setting value.
			setShading();
		}

		void GL::_initAntiAliasingPass()
		{
			_aaShader = VTXApp::get().getProgramManager().createProgram( "AA", { "shading/fxaa.frag" } );
		}

		void GL::_initQuadVAO()
		{
			// Init quad vao/vbo for deferred shading.

			// clang-format off
			GLfloat quadVertices[] =
			{
				-1.0f,  1.0f,
				-1.0f, -1.0f, 
				 1.0f,  1.0f,
				 1.0f, -1.0f,
			};
			// clang-format on

			// setup plane VAO
			glGenBuffers( 1, &_quadVBO );
			glBindBuffer( GL_ARRAY_BUFFER, _quadVBO );
			glBufferData( GL_ARRAY_BUFFER, sizeof( quadVertices ), quadVertices, GL_STATIC_DRAW );
			glGenVertexArrays( 1, &_quadVAO );
			glBindVertexArray( _quadVAO );
			glBindBuffer( GL_ARRAY_BUFFER, _quadVBO );
			glEnableVertexAttribArray( 0 );
			glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0 );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glBindVertexArray( 0 );
		}

		void GL::clear( const Object3D::Scene & p_scene )
		{
			// TODO.

			_isInitialized = false;
		}

		void GL::render( const Object3D::Scene & p_scene )
		{
			if ( _isInitialized == false ) { return; }

			glEnable( GL_DEPTH_TEST );
			_geometricPass( p_scene );
			glDisable( GL_DEPTH_TEST );

			_ssaoPass( p_scene );
			_blurPass();
			_shadingPass();
			//_antiAliasingPass();
		};

		void GL::_geometricPass( const Object3D::Scene & p_scene )
		{
			glBindFramebuffer( GL_FRAMEBUFFER, _fboGeo );
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

			for ( Model::Molecule * const molecule : p_scene.getMolecules() )
			{
				molecule->render();
			}

			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		}

		void GL::_ssaoPass( const Object3D::Scene & p_scene )
		{
			glBindFramebuffer( GL_FRAMEBUFFER, _fboSSAO );
			glClear( GL_COLOR_BUFFER_BIT );

			if ( Setting::Rendering::useSSAO == false ) return;

			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, _colorNormalCompressedTexture );
			glActiveTexture( GL_TEXTURE1 );
			glBindTexture( GL_TEXTURE_2D, _camSpacePositionsTexture );
			glActiveTexture( GL_TEXTURE2 );
			glBindTexture( GL_TEXTURE_2D, _noiseTexture );

			_ssaoShader->use();

			// TODO don't aoRadius update each frame
			glUniform1f( _uAoRadiusLoc, Setting::Rendering::aoRadius );
			glUniform1i( _uAoIntensityLoc, Setting::Rendering::aoIntensity );

			glUniformMatrix4fv(
				_uProjMatrixLoc, 1, GL_FALSE, glm::value_ptr( ( p_scene.getCamera().getProjectionMatrix() ) ) );

			glBindVertexArray( _quadVAO );
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

		void GL::_blurPass()
		{
			glBindFramebuffer( GL_FRAMEBUFFER, _fboBlur );
			glClear( GL_COLOR_BUFFER_BIT );

			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, _ssaoTexture );

			_blurShader->use();
			// TODO don't aoRadius update each frame
			glUniform1i( _uBlurSizeLoc, Setting::Rendering::aoBlurSize );

			glBindVertexArray( _quadVAO );
			glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
			glBindVertexArray( 0 );

			// TODO Needed ?
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, 0 );
		}

		void GL::_shadingPass()
		{
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			// glBindFramebuffer( GL_FRAMEBUFFER, _fboShading );

			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, _colorNormalCompressedTexture );
			glActiveTexture( GL_TEXTURE1 );
			glBindTexture( GL_TEXTURE_2D, _camSpacePositionsTexture );
			glActiveTexture( GL_TEXTURE2 );
			glBindTexture( GL_TEXTURE_2D, _blurTexture );

			_currentShading->use();

			glBindVertexArray( _quadVAO );
			glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
			glBindVertexArray( 0 );
		}

		void GL::_antiAliasingPass()
		{
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );

			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, _shadingTexture );

			_aaShader->use();

			glBindVertexArray( _quadVAO );
			glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
			glBindVertexArray( 0 );
		}

		void GL::setShading()
		{
			switch ( Setting::Rendering::shading )
			{
			case SHADING::TOON: _currentShading = _toonShading; break;
			case SHADING::BLINN_PHONG: _currentShading = _blinnPhongShading; break;
			case SHADING::LAMBERT:
			default: _currentShading = _diffuseShading;
			}
		}
	} // namespace Renderer
} // namespace VTX
