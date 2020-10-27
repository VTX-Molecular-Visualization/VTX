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
				gl()->glDeleteFramebuffers( 1, &_fbo );
				gl()->glDeleteTextures( 1, &_viewPositionsNormalsCompressedTexture );
				gl()->glDeleteTextures( 1, &_colorsTexture );
				gl()->glDeleteTextures( 1, &_depthTexture );
			}

			void Geometric::init( GLSL::ProgramManager & p_programManager, const uint p_width, const uint p_height )
			{
				// TODO: Only when using point sprites.
				/*glEnable( GL_PROGRAM_POINT_SIZE );
				glPointParameteri( GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT );*/

				// Create G-buffers for deferred shading.
				gl()->glGenFramebuffers( 1, &_fbo );

				gl()->glBindFramebuffer( GL_FRAMEBUFFER, _fbo );

				gl()->glGenTextures( 1, &_viewPositionsNormalsCompressedTexture );
				gl()->glBindTexture( GL_TEXTURE_2D, _viewPositionsNormalsCompressedTexture );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				gl()->glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA32UI, p_width, p_height, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, nullptr );

				gl()->glGenTextures( 1, &_colorsTexture );
				gl()->glBindTexture( GL_TEXTURE_2D, _colorsTexture );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				gl()->glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, nullptr );

				gl()->glGenTextures( 1, &_depthTexture );
				gl()->glBindTexture( GL_TEXTURE_2D, _depthTexture );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				gl()->glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				gl()->glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, p_width, p_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr );

				gl()->glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _viewPositionsNormalsCompressedTexture, 0 );
				gl()->glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _colorsTexture, 0 );
				gl()->glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthTexture, 0 );

				static const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

				gl()->glDrawBuffers( 2, drawBuffers );

				gl()->glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				GLenum fboStatus = gl()->glCheckFramebufferStatus( GL_FRAMEBUFFER );
				if ( fboStatus != GL_FRAMEBUFFER_COMPLETE )
				{
					VTX_WARNING( "Framebuffer not complete: " + std::to_string( fboStatus ) );
				}

				GLenum glstatus = gl()->glGetError();
				if ( glstatus != GL_NO_ERROR )
				{
					VTX_ERROR( "Error in GL call: " + std::to_string( glstatus ) );
				}
			}

			void Geometric::resize( const uint p_width, const uint p_height )
			{
				gl()->glBindTexture( GL_TEXTURE_2D, _viewPositionsNormalsCompressedTexture );
				gl()->glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA32UI, p_width, p_height, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, nullptr );

				gl()->glBindTexture( GL_TEXTURE_2D, _colorsTexture );
				gl()->glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, p_width, p_height, 0, GL_RGBA, GL_FLOAT, nullptr );

				gl()->glBindTexture( GL_TEXTURE_2D, _depthTexture );
				gl()->glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, p_width, p_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr );
			}

			void Geometric::render( const Object3D::Scene & p_scene, const Renderer::GL & p_renderer )
			{
				gl()->glBindFramebuffer( GL_FRAMEBUFFER, _fbo );
				gl()->glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

				for ( const Object3D::Scene::PairMoleculePtrFloat & pair : p_scene.getMolecules() )
				{
					pair.first->render();
				}
				for ( const Object3D::Scene::MeshTrianglePtr & mesh : p_scene.getMeshes() )
				{
					mesh->render();
				}

				gl()->glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			}

		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX
