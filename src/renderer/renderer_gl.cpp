#include "renderer_gl.hpp"
#include "glm/gtx/compatibility.hpp"
#include "model/model_molecule.hpp"
#include "setting.hpp"
#include "view/base_view_3d_molecule.hpp"
#include "vtx_app.hpp"
#include <random>

namespace VTX
{
	namespace Renderer
	{
		void RendererGL::init( Object3D::Scene & p_scene, uint p_width, uint p_height )
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

		void RendererGL::_initGeometricPass()
		{
			VTX_INFO( std::to_string( _width ) + " x " + std::to_string( _height ) );
			glEnable( GL_DEPTH_TEST );
			glClearDepth( 1.f );
			glDepthFunc( GL_LESS );
			glEnable( GL_PROGRAM_POINT_SIZE );
			glPointParameteri( GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT );
			glViewport( 0, 0, _width, _height );
			// glEnable(GL_BLEND);
			// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
			glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGBA16F, _width, _height );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

			glGenTextures( 1, &_depthTexture );
			glBindTexture( GL_TEXTURE_2D, _depthTexture );
			glTexStorage2D( GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, _width, _height );

			glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _colorNormalCompressedTexture, 0 );
			glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _camSpacePositionsTexture, 0 );
			glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthTexture, 0 );

			static const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

			glDrawBuffers( 2, drawBuffers );

			glBindFramebuffer( GL_FRAMEBUFFER, 0 );

			GLenum fboStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );
			if ( fboStatus != GL_FRAMEBUFFER_COMPLETE ) { VTX_WARNING( "Framebuffer not complete: " + fboStatus ); }

			GLenum glstatus = glGetError();
			if ( glstatus != GL_NO_ERROR ) { VTX_ERROR( "Error in GL call: " + glstatus ); }
		}

		void RendererGL::_initSsaoPass()
		{
			glGenFramebuffers( 1, &_fboSSAO );
			glBindFramebuffer( GL_FRAMEBUFFER, _fboSSAO );

			glGenTextures( 1, &_ssaoTexture );
			glBindTexture( GL_TEXTURE_2D, _ssaoTexture );
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, _width, _height, 0, GL_RGB, GL_FLOAT, NULL );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ssaoTexture, 0 );
			static const GLenum drawBufferSSAO[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers( 1, drawBufferSSAO );

			_ssaoShader = VTXApp::get().getProgramManager().createProgram( "SSAO" );
			_ssaoShader->attachShader( VTXApp::get().getProgramManager().createShader( "shading/ssao.frag" ) );
			_ssaoShader->link();

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

		void RendererGL::_initBlurPass()
		{
			glGenFramebuffers( 1, &_fboBlur );
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

			_blurShader = VTXApp::get().getProgramManager().createProgram( "Blur" );
			_blurShader->attachShader( VTXApp::get().getProgramManager().createShader( "shading/blur.frag" ) );
			_blurShader->link();

			_uBlurSizeLoc = glGetUniformLocation( _blurShader->getId(), "uBlurSize" );

			_blurShader->use();
			glUniform1i( _uBlurSizeLoc, Setting::Rendering::aoBlurSize );
		}

		void RendererGL::_initShadingPass()
		{
			GL::GLSLProgramManager & programManager = VTXApp::get().getProgramManager();

			glGenFramebuffers( 1, &_fboShading );
			glBindFramebuffer( GL_FRAMEBUFFER, _fboShading );

			glGenTextures( 1, &_shadingTexture );
			glBindTexture( GL_TEXTURE_2D, _shadingTexture );
			glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGBA32F, _width, _height );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

			glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _shadingTexture, 0 );
			static const GLenum draw_bufferShading[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers( 1, draw_bufferShading );
			_toonShading = programManager.createProgram( "ToonShading" );
			_toonShading->attachShader( programManager.createShader( "shading/toonShading.frag" ) );
			_toonShading->link();
			_diffuseShading = programManager.createProgram( "DiffuseShading" );
			_diffuseShading->attachShader( programManager.createShader( "shading/diffuseShading.frag" ) );
			_diffuseShading->link();
			_blinnPhongShading = programManager.createProgram( "BlinnPhongShading" );
			_blinnPhongShading->attachShader( programManager.createShader( "shading/blinnPhongShading.frag" ) );
			_blinnPhongShading->link();

			// Use setting value.
			setShading();
		}

		void RendererGL::_initAntiAliasingPass()
		{
			_aaShader = VTXApp::get().getProgramManager().createProgram( "AA" );
			_aaShader->attachShader( VTXApp::get().getProgramManager().createShader( "shading/fxaa.frag" ) );
			_aaShader->link();
		}

		void RendererGL::_initQuadVAO()
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

		void RendererGL::clear( Object3D::Scene & p_scene )
		{
			// TODO.

			_isInitialized = false;
		}

		void RendererGL::render( Object3D::Scene & p_scene )
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

		void RendererGL::_geometricPass( Object3D::Scene & p_scene )
		{
			glBindFramebuffer( GL_FRAMEBUFFER, _fboGeo );
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

			for ( Model::ModelMolecule * const molecule : p_scene.getMolecules() )
			{
				molecule->render();
			}

			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		}

		void RendererGL::_ssaoPass( Object3D::Scene & p_scene )
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
			glActiveTexture( GL_TEXTURE3 );
			glBindTexture( GL_TEXTURE_2D, _depthTexture );

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

		void RendererGL::_blurPass()
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

		void RendererGL::_shadingPass()
		{
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			// glBindFramebuffer(GL_FRAMEBUFFER, m_fboShading);

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

		void RendererGL::_antiAliasingPass()
		{
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );

			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, _shadingTexture );

			_aaShader->use();

			glBindVertexArray( _quadVAO );
			glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
			glBindVertexArray( 0 );
		}

		void RendererGL::setShading()
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
