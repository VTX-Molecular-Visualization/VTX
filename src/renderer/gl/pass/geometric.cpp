#include "geometric.hpp"
#include "renderer/gl/gl.hpp"

namespace VTX
{
	namespace Renderer
	{
		namespace Pass
		{
			Geometric::~Geometric()
			{
				glDeleteFramebuffers( 1, &_fbo );
				glDeleteTextures( 1, &_viewPositionsNormalsCompressedTexture );
				glDeleteTextures( 1, &_colorsTexture );
				glDeleteTextures( 1, &_depthTexture );
			}

			void Geometric::init( GLSL::ProgramManager & p_programManager, const uint p_width, const uint p_height )
			{
				// TODO: Only when using point sprites.
				/*glEnable( GL_PROGRAM_POINT_SIZE );
				glPointParameteri( GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT );*/

				// Create G-buffers for deferred shading.
				glGenFramebuffers( 1, &_fbo );

				glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				glGenTextures( 1, &_viewPositionsNormalsCompressedTexture );
				glBindTexture( GL_TEXTURE_2D, _viewPositionsNormalsCompressedTexture );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				glTexImage2D(
					GL_TEXTURE_2D, 0, GL_RGBA32UI, p_width, p_height, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, nullptr );

				glGenTextures( 1, &_colorsTexture );
				glBindTexture( GL_TEXTURE_2D, _colorsTexture );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, nullptr );

				glGenTextures( 1, &_depthTexture );
				glBindTexture( GL_TEXTURE_2D, _depthTexture );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				glTexImage2D( GL_TEXTURE_2D,
							  0,
							  GL_DEPTH_COMPONENT32F,
							  p_width,
							  p_height,
							  0,
							  GL_DEPTH_COMPONENT,
							  GL_FLOAT,
							  nullptr );

				glFramebufferTexture2D(
					GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _viewPositionsNormalsCompressedTexture, 0 );
				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _colorsTexture, 0 );
				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthTexture, 0 );

				static const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

				glDrawBuffers( 2, drawBuffers );

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				GLenum fboStatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );
				if ( fboStatus != GL_FRAMEBUFFER_COMPLETE )
				{
					VTX_WARNING( "Framebuffer not complete: " + fboStatus );
				}

				GLenum glstatus = glGetError();
				if ( glstatus != GL_NO_ERROR )
				{
					VTX_ERROR( "Error in GL call: " + glstatus );
				}
			}

			void Geometric::resize( const uint p_width, const uint p_height )
			{
				glBindTexture( GL_TEXTURE_2D, _viewPositionsNormalsCompressedTexture );
				glTexImage2D(
					GL_TEXTURE_2D, 0, GL_RGBA32UI, p_width, p_height, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, nullptr );

				glBindTexture( GL_TEXTURE_2D, _colorsTexture );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, nullptr );

				glBindTexture( GL_TEXTURE_2D, _depthTexture );
				glTexImage2D( GL_TEXTURE_2D,
							  0,
							  GL_DEPTH_COMPONENT32F,
							  p_width,
							  p_height,
							  0,
							  GL_DEPTH_COMPONENT,
							  GL_FLOAT,
							  nullptr );
			}

			void Geometric::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				glBindFramebuffer( GL_FRAMEBUFFER, _fbo );
				glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

				for ( const Object3D::Scene::PairMoleculePtrFloat & pair : p_scene.getMolecules() )
				{
					pair.first->render();
				}
				for ( const Object3D::Scene::MeshTrianglePtr & mesh : p_scene.getMeshes() )
				{
					mesh->render();
				}

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			}

		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
