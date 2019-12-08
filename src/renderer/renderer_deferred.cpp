#include "renderer_deferred.hpp"
#include "../model/model_molecule.hpp"
#include "../view/view_3d_ball_and_stick.hpp"
#include "base_renderer.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <random>

namespace VTX
{
	namespace Renderer
	{
		void RendererDeferred::init( Object3D::Scene & p_scene )
		{
			VTX_INFO( "Initializing renderer..." );

			// Clear.
			clear( p_scene );

			// Init pass.
			_initGeometricPass();
			_initSsaoPass();
			_initBlurPass();
			_initShadingPass();
			_initAntiAliasingPass();
			_initQuadVAO();

			// Init views.
			for ( Model::ModelMolecule * const molecule : p_scene.getMolecules() )
			{
				View::BaseView3DMolecule * view = (View::BaseView3DMolecule *)molecule->getCurrentView3D();
				view->setupShaders( _programManager );
			}

			_isInitialized = true;
			VTX_INFO( "Renderer initialized" );
			p_scene.getCameraOrbit().printInfo();
		}

		inline void RendererDeferred::_initGeometricPass()
		{
			glEnable( GL_DEPTH_TEST );
			glClearDepth( 1.f );
			glDepthFunc( GL_LESS );
			glEnable( GL_PROGRAM_POINT_SIZE );
			glPointParameteri( GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT );
			glViewport( 0, 0, _width, _height );
			// glEnable(GL_BLEND);
			// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// create G-buffers for deferred shading
			glGenFramebuffers( 1, &_fboGeo );
			glBindFramebuffer( GL_FRAMEBUFFER, _fboGeo );

			glGenTextures( 1, &_colorNormalCompressedTexture );
			glBindTexture( GL_TEXTURE_2D, _colorNormalCompressedTexture );
			glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGBA32UI, _width, _height );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

			glGenTextures( 1, &_camSpacePositionsTexture );
			glBindTexture( GL_TEXTURE_2D, _camSpacePositionsTexture );
			glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGBA32F, _width, _height );
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
		}

		inline void RendererDeferred::_initSsaoPass()
		{
			glGenFramebuffers( 1, &_fboSSAO );
			glBindFramebuffer( GL_FRAMEBUFFER, _fboSSAO );

			glGenTextures( 1, &_ssaoTexture );
			glBindTexture( GL_TEXTURE_2D, _ssaoTexture );
			glTexStorage2D( GL_TEXTURE_2D, 1, GL_R32F, _width, _height );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

			glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _ssaoTexture, 0 );
			static const GLenum draw_bufferSSAO[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers( 1, draw_bufferSSAO );

			_ssaoShader = _programManager.createProgram( "SSAO" );
			_ssaoShader->attachShader( _programManager.createShader( "shading/ssao.frag" ) );
			_ssaoShader->link();

			_uProjMatrixLoc = glGetUniformLocation( _ssaoShader->getId(), "uProjMatrix" );
			_uAoKernelLoc	= glGetUniformLocation( _ssaoShader->getId(), "uAoKernel" );

			// generate random ao kernel
			std::random_device							 rd;
			static std::mt19937							 gen( 0 ); // rd());
			static std::uniform_real_distribution<float> dist( 0.f, 1.f );
			const uint									 kernelSize = 32;
			Vec3f										 aoKernel[ kernelSize ];
			for ( uint i = 0; i < kernelSize; i++ )
			{
				// std::cout << "adaptative ? " << std::endl;
				Vec3f v;
				// sample on unit hemisphere
				v.x = 2.f * dist( gen ) - 1.f;
				v.y = 2.f * dist( gen ) - 1.f;
				v.z = dist( gen );
				v	= glm::normalize( v );
				// scale sample within the hemisphere
				v *= dist( gen );
				// accelerating interpolation (distance from center reduces when
				// number of points grow up)
				float scale = float( i ) / float( kernelSize );
				scale		= 0.1f + 0.9f * scale * scale; // lerp
				v *= scale;
				aoKernel[ i ] = v;
			}

			_ssaoShader->use();
			glUniform3fv( _uAoKernelLoc, kernelSize, (const GLfloat *)aoKernel );

			// generate noise texture
			const uint		   noiseTextureSize = 128;
			std::vector<Vec3f> noise( noiseTextureSize * noiseTextureSize );

			for ( uint i = 0; i < noiseTextureSize * noiseTextureSize; ++i )
			{
				noise[ i ] = Vec3f( dist( gen ), dist( gen ), 0.f );
				noise[ i ] = glm::normalize( noise[ i ] );
			}
			glGenTextures( 1, &_noiseTexture );
			glBindTexture( GL_TEXTURE_2D, _noiseTexture );
			glTexImage2D(
				GL_TEXTURE_2D, 0, GL_RGB16F, noiseTextureSize, noiseTextureSize, 0, GL_RGB, GL_FLOAT, noise.data() );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			// repeat tile over the image
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		}

		inline void RendererDeferred::_initBlurPass()
		{
			glGenFramebuffers( 1, &_fboBlur );
			glBindFramebuffer( GL_FRAMEBUFFER, _fboBlur );

			glGenTextures( 1, &_blurTexture );
			glBindTexture( GL_TEXTURE_2D, _blurTexture );
			glTexStorage2D( GL_TEXTURE_2D, 1, GL_R32F, _width, _height );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

			glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _blurTexture, 0 );
			static const GLenum draw_bufferBlur[] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers( 1, draw_bufferBlur );

			_blurShader = _programManager.createProgram( "Blur" );
			_blurShader->attachShader( _programManager.createShader( "shading/blur.frag" ) );
			_blurShader->link();
		}

		inline void RendererDeferred::_initShadingPass()
		{
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
			_toonShading = _programManager.createProgram( "ToonShading" );
			_toonShading->attachShader( _programManager.createShader( "shading/toonShading.frag" ) );
			_toonShading->link();
			_diffuseShading = _programManager.createProgram( "DiffuseShading" );
			_diffuseShading->attachShader( _programManager.createShader( "shading/diffuseShading.frag" ) );
			_diffuseShading->link();
			_blinnPhongShading = _programManager.createProgram( "BlinnPhongShading" );
			_blinnPhongShading->attachShader( _programManager.createShader( "shading/blinnPhongShading.frag" ) );
			_blinnPhongShading->link();

			// TODO: use settings.
			_currentShading = _diffuseShading;
		}

		inline void RendererDeferred::_initAntiAliasingPass()
		{
			_aaShader = _programManager.createProgram( "AA" );
			_aaShader->attachShader( _programManager.createShader( "shading/fxaa.frag" ) );
			_aaShader->link();
		}

		inline void RendererDeferred::_initQuadVAO()
		{
			// Init quad vao/vbo for deferred shading.

			// clang-format off
			GLfloat m_quadVertices[] =
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
			glBufferData( GL_ARRAY_BUFFER, sizeof( m_quadVertices ), m_quadVertices, GL_STATIC_DRAW );
			glGenVertexArrays( 1, &_quadVAO );
			glBindVertexArray( _quadVAO );
			glBindBuffer( GL_ARRAY_BUFFER, _quadVBO );
			glEnableVertexAttribArray( 0 );
			glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0 );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glBindVertexArray( 0 );
		}

		void RendererDeferred::clear( Object3D::Scene & p_scene )
		{
			for ( Model::ModelMolecule * const molecule : p_scene.getMolecules() )
			{
				View::BaseView3DMolecule * view = (View::BaseView3DMolecule *)molecule->getCurrentView3D();

				// TODO: delete shaders/programs or let them for further use?
			}
			_isInitialized = false;
		}

		void RendererDeferred::render( Object3D::Scene & p_scene, const uint p_time )
		{
			if ( _isInitialized == false ) { return; }

			glEnable( GL_DEPTH_TEST );
			_geometricPass( p_scene );
			glDisable( GL_DEPTH_TEST );
			_ssaoPass( p_scene );
			_blurPass();
			_shadingPass();
			_antiAliasingPass();
		};

		inline void RendererDeferred::_geometricPass( Object3D::Scene & p_scene )
		{
			glBindFramebuffer( GL_FRAMEBUFFER, _fboGeo );
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

			for ( Model::ModelMolecule * const molecule : p_scene.getMolecules() )
			{
				View::View3DBallAndStick * view = (View::View3DBallAndStick *)molecule->getCurrentView3D();

				view->bindVAO();
				view->bindIBO();

				view->useShaders( _programManager );
				view->setCameraMatrices( p_scene.getCameraOrbit().getViewMatrix(),
										 p_scene.getCameraOrbit().getProjectionMatrix() );

				view->render( 0 );
				view->unbindIBO();
				view->unbindVAO();
			}

			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		}

		inline void RendererDeferred::_ssaoPass( Object3D::Scene & p_scene )
		{
			glBindFramebuffer( GL_FRAMEBUFFER, _fboSSAO );
			glClear( GL_COLOR_BUFFER_BIT );

			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, _colorNormalCompressedTexture );
			glActiveTexture( GL_TEXTURE1 );
			glBindTexture( GL_TEXTURE_2D, _camSpacePositionsTexture );
			glActiveTexture( GL_TEXTURE2 );
			glBindTexture( GL_TEXTURE_2D, _noiseTexture );
			glActiveTexture( GL_TEXTURE3 );
			glBindTexture( GL_TEXTURE_2D, _depthTexture );

			_ssaoShader->use();

			glUniformMatrix4fv(
				_uProjMatrixLoc, 1, GL_FALSE, glm::value_ptr( ( p_scene.getCameraOrbit().getProjectionMatrix() ) ) );

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

		inline void RendererDeferred::_blurPass()
		{
			glBindFramebuffer( GL_FRAMEBUFFER, _fboBlur );
			glClear( GL_COLOR_BUFFER_BIT );

			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, _ssaoTexture );

			_blurShader->use();

			glBindVertexArray( _quadVAO );
			glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
			glBindVertexArray( 0 );

			// TODO Needed ?
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, 0 );
		}

		inline void RendererDeferred::_shadingPass()
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

		inline void RendererDeferred::_antiAliasingPass()
		{
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );

			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, _shadingTexture );

			_aaShader->use();

			glBindVertexArray( _quadVAO );
			glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
			glBindVertexArray( 0 );
		}

	} // namespace Renderer
} // namespace VTX
