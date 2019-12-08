#include "renderer_deferred.hpp"
#include "../model/model_molecule.hpp"
#include "../view/base_view_3d_molecule.hpp"
#include "base_renderer.hpp"

namespace VTX
{
	namespace Renderer
	{
		void RendererDeferred::init( const Object3D::Scene & p_scene )
		{
			VTX_INFO( "Initializing renderer..." );

			// Clear.
			clear( p_scene );

			// Init pass.
			_initGeometricPass();
			_initBlurPass();
			_initShadingPass();
			_initQuadVAO();

			// Init views.
			for ( Model::ModelMolecule * const molecule : p_scene.getMolecules() )
			{
				View::BaseView3DMolecule * view = (View::BaseView3DMolecule *)molecule->getCurrentView3D();

				// Create program.
				Shader::GLSLProgram * program = _programManager.createProgram( view->getProgramName() );
				for ( std::string shader : view->getShaderNames() )
				{
					program->attachShader( _programManager.createShader( shader ) );
				}

				program->link();
				view->setupUniforms( program->getId() );
			}

			_isInitialized = true;
		} // namespace RendererDeferred::init(constObject3D::Scene&,p_scene)

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

		// inline void RendererDeferred::_initSsaoPass() {}

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

		// inline void RendererDeferred::_initAntiAliasingPass() {}

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

		void RendererDeferred::clear( const Object3D::Scene & p_scene )
		{
			for ( Model::ModelMolecule * const molecule : p_scene.getMolecules() )
			{
				View::BaseView3DMolecule * view = (View::BaseView3DMolecule *)molecule->getCurrentView3D();

				// Delete program.
				_programManager.deleteProgram( view->getProgramName() );
			}
			_isInitialized = false;
		}

		void RendererDeferred::render( Object3D::Scene & p_scene, const uint p_time )
		{
			if ( _isInitialized == false ) { return; }

			//glEnable( GL_DEPTH_TEST );
			_geometricPass( p_scene );
			//glDisable( GL_DEPTH_TEST );
			//_ssaoPass();
			_blurPass();
			_shadingPass();
			// _antiAliasingPass();
		};

		inline void RendererDeferred::_geometricPass( Object3D::Scene & p_scene )
		{
			glBindFramebuffer( GL_FRAMEBUFFER, _fboGeo );
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

			for ( Model::ModelMolecule * const molecule : p_scene.getMolecules() )
			{
				View::BaseView3DMolecule * view = (View::BaseView3DMolecule *)molecule->getCurrentView3D();

				view->bindVAO();
				view->bindIBO();

				view->setCameraMatrices( p_scene.getCameraOrbit().getViewMatrix(),
										 p_scene.getCameraOrbit().getProjectionMatrix() );

				_programManager.getProgram( view->getProgramName() )->use();
				view->render( 0 );
				view->unbindIBO();
				view->unbindVAO();
			}

			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		}

		// inline void RendererDeferred::_ssaoPass() {}

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

		// inline void RendererDeferred::_antiAliasingPass() {}

	} // namespace Renderer
} // namespace VTX
