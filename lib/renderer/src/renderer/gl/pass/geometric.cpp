#include "renderer/gl/pass/geometric.hpp"
#include <util/color/rgba.hpp>
#include <util/logger.hpp>

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

		///////////////// Triangles test.
		in.triangles.vboPositions.create();
		in.triangles.vboNormals.create();
		in.triangles.vboColors.create();
		in.triangles.vboVisibilities.create();
		in.triangles.vboSelections.create();
		in.triangles.vboIds.create();
		in.triangles.ebo.create();

		in.triangles.vao.create();

		in.triangles.vao.bindElementBuffer( in.triangles.ebo );

		// Position.
		in.triangles.vao.enableAttribute( 0 );
		in.triangles.vao.setVertexBuffer<float>( 0, in.triangles.vboPositions, sizeof( Vec3f ) );
		in.triangles.vao.setAttributeFormat<float>( 0, 3 );
		in.triangles.vao.setAttributeBinding( 0, 0 );

		// Normal.
		in.triangles.vao.enableAttribute( 1 );
		in.triangles.vao.setVertexBuffer<float>( 1, in.triangles.vboNormals, sizeof( Vec3f ) );
		in.triangles.vao.setAttributeFormat<float>( 1, 3 );
		in.triangles.vao.setAttributeBinding( 1, 1 );

		// Color.
		in.triangles.vao.enableAttribute( 2 );
		in.triangles.vao.setVertexBuffer<float>( 2, in.triangles.vboColors, sizeof( Util::Color::Rgba ) );
		in.triangles.vao.setAttributeFormat<float>( 2, 4 );
		in.triangles.vao.setAttributeBinding( 2, 2 );

		// Visbility.
		in.triangles.vao.enableAttribute( 3 );
		in.triangles.vao.setVertexBuffer<uint>( 3, in.triangles.vboVisibilities, sizeof( uint ) );
		in.triangles.vao.setAttributeFormat<uint>( 3, 1 );
		in.triangles.vao.setAttributeBinding( 3, 3 );

		// Selection.
		in.triangles.vao.enableAttribute( 4 );
		in.triangles.vao.setVertexBuffer<uint>( 4, in.triangles.vboSelections, sizeof( uint ) );
		in.triangles.vao.setAttributeFormat<uint>( 4, 1 );
		in.triangles.vao.setAttributeBinding( 4, 4 );

		// Id.
		in.triangles.vao.enableAttribute( 5 );
		in.triangles.vao.setVertexBuffer<uint>( 5, in.triangles.vboIds, sizeof( uint ) );
		in.triangles.vao.setAttributeFormat<uint>( 5, 1 );
		in.triangles.vao.setAttributeBinding( 5, 5 );

		in.triangles.vboPositions.set(
			std::vector<Vec3f> { Vec3f( 0.5f, -0.5f, 0.f ), Vec3f( -0.5f, -0.5f, 0.f ), Vec3f( 0.f, 0.5f, 0.f ) } );
		in.triangles.vboNormals.set(
			std::vector<Vec3f> { Vec3f( 0.f, 0.f, 1.f ), Vec3f( 0.f, 0.f, 1.f ), Vec3f( 0.f, 0.f, 1.f ) } );
		in.triangles.vboColors.set( std::vector<Util::Color::Rgba> { Util::Color::Rgba( 1.f, 0.f, 0.f, 1.f ),
																	 Util::Color::Rgba( 0.f, 1.f, 0.f, 1.f ),
																	 Util::Color::Rgba( 0.f, 0.f, 1.f, 1.f ) } );
		in.triangles.vboVisibilities.set( std::vector<uint> { 1, 1, 1, 1 } );
		in.triangles.vboSelections.set( std::vector<uint> { 0, 0, 0, 0 } );
		in.triangles.vboIds.set( std::vector<uint> { 0, 0, 0, 0 } );
		in.triangles.ebo.set( std::vector<uint> { 0, 1, 2 } );
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
		_programTriangle->use();
		in.triangles.vao.drawElement( GL_TRIANGLES, 3, GL_UNSIGNED_INT );

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
