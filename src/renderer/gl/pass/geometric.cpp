#include "geometric.hpp"
#include "model/mesh_triangle.hpp"
#include "model/molecule.hpp"
#include "renderer/gl/gl.hpp"

namespace VTX::Renderer::GL::Pass
{
	Geometric::~Geometric()
	{
		gl()->glDeleteFramebuffers( 1, &_fbo );
		gl()->glDeleteTextures( 1, &_viewPositionsNormalsCompressedTexture );
		gl()->glDeleteTextures( 1, &_colorsTexture );
		gl()->glDeleteTextures( 1, &_depthTexture );
	}

	void Geometric::init( const uint p_width, const uint p_height )
	{
		// TODO: Only when using point sprites.
		/*glEnable( GL_PROGRAM_POINT_SIZE );
		glPointParameteri( GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT );*/

		// Create G-buffers for deferred shading.
		gl()->glCreateFramebuffers( 1, &_fbo );

		gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_viewPositionsNormalsCompressedTexture );
		gl()->glTextureParameteri( _viewPositionsNormalsCompressedTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _viewPositionsNormalsCompressedTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _viewPositionsNormalsCompressedTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		gl()->glTextureParameteri( _viewPositionsNormalsCompressedTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		gl()->glTextureStorage2D( _viewPositionsNormalsCompressedTexture, 1, GL_RGBA32UI, p_width, p_height );

		gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_colorsTexture );
		gl()->glTextureParameteri( _colorsTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _colorsTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _colorsTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		gl()->glTextureParameteri( _colorsTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		gl()->glTextureStorage2D( _colorsTexture, 1, GL_RGBA16F, p_width, p_height );

		gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_depthTexture );
		gl()->glTextureParameteri( _depthTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _depthTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _depthTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		gl()->glTextureParameteri( _depthTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		gl()->glTextureStorage2D( _depthTexture, 1, GL_DEPTH_COMPONENT32F, p_width, p_height );

		gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _viewPositionsNormalsCompressedTexture, 0 );
		gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT1, _colorsTexture, 0 );
		gl()->glNamedFramebufferTexture( _fbo, GL_DEPTH_ATTACHMENT, _depthTexture, 0 );

		static const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		gl()->glNamedFramebufferDrawBuffers( _fbo, 2, drawBuffers );

		GLenum fboStatus = gl()->glCheckNamedFramebufferStatus( _fbo, GL_FRAMEBUFFER );
		if ( fboStatus != GL_FRAMEBUFFER_COMPLETE )
		{
			VTX_WARNING( "Framebuffer not complete: " + std::to_string( fboStatus ) );
		}
	}

	void Geometric::resize( const uint p_width, const uint p_height )
	{
		gl()->glDeleteTextures( 1, &_viewPositionsNormalsCompressedTexture );
		gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_viewPositionsNormalsCompressedTexture );
		gl()->glTextureParameteri( _viewPositionsNormalsCompressedTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _viewPositionsNormalsCompressedTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _viewPositionsNormalsCompressedTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		gl()->glTextureParameteri( _viewPositionsNormalsCompressedTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		gl()->glTextureStorage2D( _viewPositionsNormalsCompressedTexture, 1, GL_RGBA32UI, p_width, p_height );

		gl()->glDeleteTextures( 1, &_colorsTexture );
		gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_colorsTexture );
		gl()->glTextureParameteri( _colorsTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _colorsTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _colorsTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		gl()->glTextureParameteri( _colorsTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		gl()->glTextureStorage2D( _colorsTexture, 1, GL_RGBA16F, p_width, p_height );

		gl()->glCreateTextures( GL_TEXTURE_2D, 1, &_depthTexture );
		gl()->glTextureParameteri( _depthTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _depthTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		gl()->glTextureParameteri( _depthTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		gl()->glTextureParameteri( _depthTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		gl()->glTextureStorage2D( _depthTexture, 1, GL_DEPTH_COMPONENT32F, p_width, p_height );

		gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT0, _viewPositionsNormalsCompressedTexture, 0 );
		gl()->glNamedFramebufferTexture( _fbo, GL_COLOR_ATTACHMENT1, _colorsTexture, 0 );
		gl()->glNamedFramebufferTexture( _fbo, GL_DEPTH_ATTACHMENT, _depthTexture, 0 );
	}

	void Geometric::render( const Object3D::Scene & p_scene, const GL & p_renderer )
	{
		gl()->glBindFramebuffer( GL_FRAMEBUFFER, _fbo );
		gl()->glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		for ( const Object3D::Scene::PairMoleculePtrFloat & pair : p_scene.getMolecules() )
		{
			pair.first->render( p_scene.getCamera() );
		}
		for ( const Object3D::Scene::MeshTrianglePtr & mesh : p_scene.getMeshes() )
		{
			mesh->render( p_scene.getCamera() );
		}

		gl()->glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

} // namespace VTX::Renderer::GL::Pass
