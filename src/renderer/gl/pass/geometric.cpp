#include "geometric.hpp"
#include "renderer/gl/gl.hpp"

namespace VTX
{
	namespace Renderer
	{
		namespace Pass
		{
			void Geometric::init( GLSL::ProgramManager & p_programManager, const uint p_width, const uint p_height )
			{
				// TODO: Only when using point sprites.
				glEnable( GL_PROGRAM_POINT_SIZE );
				glPointParameteri( GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT );
				glViewport( 0, 0, p_width, p_height );

				// Create G-buffers for deferred shading.
				glGenFramebuffers( 1, &_fboGeo );

				glBindFramebuffer( GL_FRAMEBUFFER, _fboGeo );

				glGenTextures( 1, &_colorNormalCompressedTexture );
				glBindTexture( GL_TEXTURE_2D, _colorNormalCompressedTexture );
				glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGBA32UI, p_width, p_height );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

				glGenTextures( 1, &_camSpacePositionsTexture );
				glBindTexture( GL_TEXTURE_2D, _camSpacePositionsTexture );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, NULL );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

				glGenTextures( 1, &_depthTexture );
				glBindTexture( GL_TEXTURE_2D, _depthTexture );
				glTexStorage2D( GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, p_width, p_height );

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
			}

			void Geometric::clean()
			{
				glDeleteFramebuffers( 1, &_fboGeo );
				glDeleteTextures( 1, &_depthTexture );
				glDeleteTextures( 1, &_colorNormalCompressedTexture );
				glDeleteTextures( 1, &_camSpacePositionsTexture );
			}

			void Geometric::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				glBindFramebuffer( GL_FRAMEBUFFER, _fboGeo );
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
