#include "app/render/renderer/gl/pass/geometric.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/object3d/mesh_triangle.hpp"
#include "app/render/renderer/gl/gl.hpp"
#include "app/render/renderer/gl/program_manager.hpp"

namespace VTX::App::Render::Renderer::GL::Pass
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

		_pickingTexture.create( p_width,
								p_height,
								Texture2D::InternalFormat::RG32UI,
								Texture2D::Wrapping::CLAMP_TO_EDGE,
								Texture2D::Wrapping::CLAMP_TO_EDGE,
								Texture2D::Filter::NEAREST,
								Texture2D::Filter::NEAREST );

		_fbo.create( Framebuffer::Target::DRAW_FRAMEBUFFER );
		_fbo.attachTexture( _viewPositionsNormalsCompressedTexture, Framebuffer::Attachment::COLOR0 );
		_fbo.attachTexture( _colorsTexture, Framebuffer::Attachment::COLOR1 );
		_fbo.attachTexture( _depthTexture, Framebuffer::Attachment::DEPTH );
		_fbo.attachTexture( _pickingTexture, Framebuffer::Attachment::COLOR2 );

		_fbo.setDrawBuffers(
			{ Framebuffer::Attachment::COLOR0, Framebuffer::Attachment::COLOR1, Framebuffer::Attachment::COLOR2 } );
	}

	void Geometric::resize( const uint p_width, const uint p_height, const GL & )
	{
		_viewPositionsNormalsCompressedTexture.resize( p_width, p_height );
		_colorsTexture.resize( p_width, p_height );
		_depthTexture.resize( p_width, p_height );
		_pickingTexture.resize( p_width, p_height );

		_fbo.attachTexture( _viewPositionsNormalsCompressedTexture, Framebuffer::Attachment::COLOR0 );
		_fbo.attachTexture( _colorsTexture, Framebuffer::Attachment::COLOR1 );
		_fbo.attachTexture( _depthTexture, Framebuffer::Attachment::DEPTH );
		_fbo.attachTexture( _pickingTexture, Framebuffer::Attachment::COLOR2 );
	}

	void Geometric::render( const App::Application::Scene & p_scene, const GL & p_renderer )
	{
		_fbo.bind();
		_fbo.clear( Framebuffer::ClearBuffer::COLOR_DEPTH );

		//_gl->glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		for ( const App::Application::Scene::PairMoleculePtrFloat & pair : p_scene.getMolecules() )
		{
			pair.first->render( p_scene.getCamera() );
		}
		for ( const App::Application::Scene::MeshTrianglePtr & mesh : p_scene.getMeshes() )
		{
			mesh->render( p_scene.getCamera() );
		}
		for ( const App::Application::Scene::HelperPtr & helper : p_scene.getHelpers() )
		{
			helper->render( p_scene.getCamera() );
		}
		//_gl->glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}

} // namespace VTX::App::Render::Renderer::GL::Pass
