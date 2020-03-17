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
			/*
			glDeleteTextures( 1, &_colorNormalCompressedTexture );
			glDeleteTextures( 1, &_camSpacePositionsTexture );
			glDeleteTextures( 1, &_depthTexture );
			glDeleteTextures( 1, &_ssaoTexture );
			glDeleteTextures( 1, &_noiseTexture );
			glDeleteTextures( 1, &_blurTexture );
			glDeleteTextures( 1, &_shadingTexture );

			glDeleteFramebuffers( 1, &_fboGeo );
			glDeleteFramebuffers( 1, &_fboSSAO );
			glDeleteFramebuffers( 1, &_fboBlur );
			glDeleteFramebuffers( 1, &_fboShading );
			*/
		}

		void GL::init( const uint p_width, const uint p_height )
		{
			VTX_INFO( "Initializing renderer..." );

			// Set size.
			BaseRenderer::resize( _width, _height );

			// Init pass.
			_passGeometric.init( _programManager, p_width, p_height );
			_passSSAO.init( _programManager, p_width, p_height );
			_passBlur.init( _programManager, p_width, p_height );
			_passShading.init( _programManager, p_width, p_height );

			// Init VAO.
			_initQuadVAO();

			VTX_INFO( "Renderer initialized" );
		}

		void GL::resize( const uint p_width, const uint p_height )
		{
			/*
			if ( p_width != _width || p_height != _height )
			{
				BaseRenderer::resize( p_width, p_height );

				glDeleteTextures( 1, &_colorNormalCompressedTexture );
				glDeleteTextures( 1, &_camSpacePositionsTexture );
				glDeleteTextures( 1, &_depthTexture );
				glDeleteTextures( 1, &_ssaoTexture );
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
				glTexImage2D( GL_TEXTURE_2D, 0, GL_R8, _width, _height, 0, GL_RED, GL_FLOAT, NULL );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ssaoTexture, 0 );
				static const GLenum drawBufferSSAO[] = { GL_COLOR_ATTACHMENT0 };
				glDrawBuffers( 1, drawBufferSSAO );

				// Blur pass.
				glBindFramebuffer( GL_FRAMEBUFFER, _fboBlur );

				glGenTextures( 1, &_blurTexture );
				glBindTexture( GL_TEXTURE_2D, _blurTexture );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_R8, _width, _height, 0, GL_RED, GL_FLOAT, NULL );
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
			*/
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

		void GL::render( const Object3D::Scene & p_scene )
		{
			glEnable( GL_DEPTH_TEST );
			_passGeometric.render( p_scene, *this );
			glDisable( GL_DEPTH_TEST );

			_passSSAO.render( p_scene, *this );
			_passBlur.render( p_scene, *this );
			_passShading.render( p_scene, *this );
		};

		void GL::setShading() { _passShading.set(); }
	} // namespace Renderer
} // namespace VTX
