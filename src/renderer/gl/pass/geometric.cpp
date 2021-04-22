#include "geometric.hpp"
#include "model/mesh_triangle.hpp"
#include "model/molecule.hpp"
#include "renderer/gl/gl.hpp"
#include "renderer/gl/program_manager.hpp"

namespace VTX::Renderer::GL::Pass
{
	void Geometric::init( const uint p_width, const uint p_height, const GL & )
	{
		// TODO: Only when using point sprites.
		/*glEnable( GL_PROGRAM_POINT_SIZE );
		glPointParameteri( GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT );*/

		_viewPositionsNormalsCompressedTexture.create( p_width,
													   p_height,
													   Texture2D::InternalFormat::RGBA32UI,
													   Texture2D::Wrapping::CLAMP_TO_EDGE,
													   Texture2D::Wrapping::CLAMP_TO_EDGE,
													   Texture2D::Filter::NEAREST,
													   Texture2D::Filter::NEAREST );

		_colorsTexture.create( p_width,
							   p_height,
							   Texture2D::InternalFormat::RGBA16F,
							   Texture2D::Wrapping::CLAMP_TO_EDGE,
							   Texture2D::Wrapping::CLAMP_TO_EDGE,
							   Texture2D::Filter::NEAREST,
							   Texture2D::Filter::NEAREST );

		_depthTexture.create( p_width,
							  p_height,
							  Texture2D::InternalFormat::DEPTH_COMPONENT32F,
							  Texture2D::Wrapping::CLAMP_TO_EDGE,
							  Texture2D::Wrapping::CLAMP_TO_EDGE,
							  Texture2D::Filter::NEAREST,
							  Texture2D::Filter::NEAREST );

		_fbo.create();
		_fbo.attachTexture( _viewPositionsNormalsCompressedTexture, Framebuffer::Attachment::COLOR0 );
		_fbo.attachTexture( _colorsTexture, Framebuffer::Attachment::COLOR1 );
		_fbo.attachTexture( _depthTexture, Framebuffer::Attachment::DEPTH );

		_fbo.setDrawBuffers( { Framebuffer::Attachment::COLOR0, Framebuffer::Attachment::COLOR1 } );
	}

	void Geometric::resize( const uint p_width, const uint p_height, const GL & )
	{
		_viewPositionsNormalsCompressedTexture.resize( p_width, p_height );
		_colorsTexture.resize( p_width, p_height );
		_depthTexture.resize( p_width, p_height );

		_fbo.attachTexture( _viewPositionsNormalsCompressedTexture, Framebuffer::Attachment::COLOR0 );
		_fbo.attachTexture( _colorsTexture, Framebuffer::Attachment::COLOR1 );
		_fbo.attachTexture( _depthTexture, Framebuffer::Attachment::DEPTH );
	}

	void Geometric::render( const Object3D::Scene & p_scene, const GL & p_renderer )
	{
		_fbo.bind();
		gl()->glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		for ( const Object3D::Scene::PairMoleculePtrFloat & pair : p_scene.getMolecules() )
		{
			pair.first->render( p_scene.getCamera() );
		}
		for ( const Object3D::Scene::MeshTrianglePtr & mesh : p_scene.getMeshes() )
		{
			mesh->render( p_scene.getCamera() );
		}
	}

} // namespace VTX::Renderer::GL::Pass
