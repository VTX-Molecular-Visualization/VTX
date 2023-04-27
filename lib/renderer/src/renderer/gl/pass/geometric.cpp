#include "renderer/gl/pass/geometric.hpp"

namespace VTX::Renderer::GL::Pass
{
	void Geometric::init( const size_t p_width, const size_t p_height )
	{
		_viewPositionsNormalsCompressedTexture.create(
			p_width, p_height, GL_RGBA32UI, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		_colorsTexture.create(
			p_width, p_height, GL_RGBA16F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		_depthTexture.create(
			p_width, p_height, GL_DEPTH_COMPONENT32F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		_pickingTexture.create(
			p_width, p_height, GL_RG32UI, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		_fbo.create();
		_fbo.attachTexture( _viewPositionsNormalsCompressedTexture, GL_COLOR_ATTACHMENT0 );
		_fbo.attachTexture( _colorsTexture, GL_COLOR_ATTACHMENT1 );
		_fbo.attachTexture( _depthTexture, GL_DEPTH );
		_fbo.attachTexture( _pickingTexture, GL_COLOR_ATTACHMENT2 );

		_fbo.setDrawBuffers( { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 } );
	}

	void Geometric::resize( const size_t p_width, const size_t p_height )
	{
		_viewPositionsNormalsCompressedTexture.resize( p_width, p_height );
		_colorsTexture.resize( p_width, p_height );
		_depthTexture.resize( p_width, p_height );
		_pickingTexture.resize( p_width, p_height );

		_fbo.attachTexture( _viewPositionsNormalsCompressedTexture, GL_COLOR_ATTACHMENT0 );
		_fbo.attachTexture( _colorsTexture, GL_COLOR_ATTACHMENT1 );
		_fbo.attachTexture( _depthTexture, GL_DEPTH );
		_fbo.attachTexture( _pickingTexture, GL_COLOR_ATTACHMENT2 );
	}

	void Geometric::render()
	{
		glEnable( GL_DEPTH_TEST );

		_fbo.bind( GL_DRAW_FRAMEBUFFER );
		_fbo.clear( GL_DEPTH );

		//_gl->glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		/*
		for ( const Object3D::Scene::PairMoleculePtrFloat & pair : p_scene.getMolecules() )
		{
			pair.first->render( p_scene.getCamera() );
		}
		for ( const Object3D::Scene::MeshTrianglePtr & mesh : p_scene.getMeshes() )
		{
			mesh->render( p_scene.getCamera() );
		}
		for ( const Object3D::Scene::HelperPtr & helper : p_scene.getHelpers() )
		{
			helper->render( p_scene.getCamera() );
		}
		*/
		//_gl->glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glDisable( GL_DEPTH_TEST );
	}

} // namespace VTX::Renderer::GL::Pass
