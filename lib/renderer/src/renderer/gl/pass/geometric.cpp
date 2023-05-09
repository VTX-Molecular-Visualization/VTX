#include "renderer/gl/pass/geometric.hpp"

namespace VTX::Renderer::GL::Pass
{
	void Geometric::init( const size_t p_width, const size_t p_height, ProgramManager & p_pm )
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
		out.fbo.attachTexture( out.textureDepth, GL_DEPTH_ATTACHMENT );
		out.fbo.attachTexture( out.texturePicking, GL_COLOR_ATTACHMENT2 );

		out.fbo.setDrawBuffers( { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 } );

		_programSphere	 = p_pm.createProgram( "Sphere", FilePath( "sphere" ) );
		_programCylinder = p_pm.createProgram( "Cylinder", FilePath( "cylinder" ) );
		_programRibbon	 = p_pm.createProgram( "Ribbon", FilePath( "ribbon" ) );
		_programLine	 = p_pm.createProgram( "Line", FilePath( "line" ) );
		_programTriangle = p_pm.createProgram( "Triangle", FilePath( "triangle" ) );
		_programVoxel	 = p_pm.createProgram( "Voxel", FilePath( "voxel" ) );

		assert( _programSphere != nullptr );
		assert( _programCylinder != nullptr );
		assert( _programRibbon != nullptr );
		assert( _programLine != nullptr );
		assert( _programTriangle != nullptr );
		assert( _programVoxel != nullptr );
	}

	void Geometric::resize( const size_t p_width, const size_t p_height )
	{
		out.textureViewPositionsNormals.resize( p_width, p_height );
		out.textureColors.resize( p_width, p_height );
		out.textureDepth.resize( p_width, p_height );
		out.texturePicking.resize( p_width, p_height );

		out.fbo.attachTexture( out.textureViewPositionsNormals, GL_COLOR_ATTACHMENT0 );
		out.fbo.attachTexture( out.textureColors, GL_COLOR_ATTACHMENT1 );
		out.fbo.attachTexture( out.textureDepth, GL_DEPTH_ATTACHMENT );
		out.fbo.attachTexture( out.texturePicking, GL_COLOR_ATTACHMENT2 );
	}

	void Geometric::render( VertexArray & p_vao )
	{
		glEnable( GL_DEPTH_TEST );

		out.fbo.bind( GL_DRAW_FRAMEBUFFER );
		out.fbo.clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

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
		// p_vao.drawArray( GL_TRIANGLE_STRIP, 0, 4 ); // why not here?
		out.fbo.unbind();

		glDisable( GL_DEPTH_TEST );
	}

	Vec2i Geometric::getPickedData( const uint p_x, const uint p_y )
	{
		out.fbo.bind( GL_READ_FRAMEBUFFER );
		out.fbo.setReadBuffer( GL_COLOR_ATTACHMENT2 );
		Vec2i data = Vec2i( INVALID_ID, INVALID_ID );
		glReadPixels( p_x, p_y, 1, 1, GL_RG_INTEGER, GL_UNSIGNED_INT, &data );
		out.fbo.unbind();
		return data;
	}

} // namespace VTX::Renderer::GL::Pass
