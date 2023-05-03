#include "renderer/gl/pass/geometric.hpp"

namespace VTX::Renderer::GL::Pass
{
	void Geometric::init( const size_t p_width, const size_t p_height )
	{
		out.textureViewPositionsNormals.create(
			p_width, p_height, GL_RGBA32UI, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		out.textureColors.create(
			p_width, p_height, GL_RGBA16F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		out.textureDepth.create(
			p_width, p_height, GL_DEPTH_COMPONENT32F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		out.texturePicking.create(
			p_width, p_height, GL_RG32UI, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		out.fbo.create();
		out.fbo.attachTexture( out.textureViewPositionsNormals, GL_COLOR_ATTACHMENT0 );
		out.fbo.attachTexture( out.textureColors, GL_COLOR_ATTACHMENT1 );
		out.fbo.attachTexture( out.textureDepth, GL_DEPTH );
		out.fbo.attachTexture( out.texturePicking, GL_COLOR_ATTACHMENT2 );

		out.fbo.setDrawBuffers( { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 } );
	}

	void Geometric::resize( const size_t p_width, const size_t p_height )
	{
		out.textureViewPositionsNormals.resize( p_width, p_height );
		out.textureColors.resize( p_width, p_height );
		out.textureDepth.resize( p_width, p_height );
		out.texturePicking.resize( p_width, p_height );

		out.fbo.attachTexture( out.textureViewPositionsNormals, GL_COLOR_ATTACHMENT0 );
		out.fbo.attachTexture( out.textureColors, GL_COLOR_ATTACHMENT1 );
		out.fbo.attachTexture( out.textureDepth, GL_DEPTH );
		out.fbo.attachTexture( out.texturePicking, GL_COLOR_ATTACHMENT2 );
	}

	void Geometric::render()
	{
		glEnable( GL_DEPTH_TEST );

		out.fbo.bind( GL_DRAW_FRAMEBUFFER );
		out.fbo.clear( GL_DEPTH );

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
