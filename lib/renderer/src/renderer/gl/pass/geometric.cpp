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

		const FilePath pathSphere = FilePath( "sphere" );
		_programSphere			  = p_pm.createProgram(
			   "Sphere", { pathSphere / "sphere.vert", pathSphere / "sphere.geom", pathSphere / "sphere.frag" } );
		_programCylinder = p_pm.createProgram( "Cylinder",
											   { FilePath( "cylinder/cylinder.vert" ),
												 FilePath( "cylinder/cylinder.geom" ),
												 FilePath( "cylinder/cylinder.frag" ) } );
		_programRibbon	 = p_pm.createProgram( "Ribbon",
											   { FilePath( "ribbon/ribbon_patch.vert" ),
												 FilePath( "ribbon/ribbon_patch.tesc" ),
												 FilePath( "ribbon/ribbon_patch.tese" ),
												 FilePath( "ribbon/ribbon_patch.frag" ) } );
		_programLine	 = p_pm.createProgram( "Line", { FilePath( "line/line.vert" ), FilePath( "line/line.frag" ) } );
		_programTriangle = p_pm.createProgram(
			"Triangle", { FilePath( "triangle/triangle.vert" ), FilePath( "triangle/triangle.frag" ) } );
		_programVoxel = p_pm.createProgram(
			"Voxel",
			{ FilePath( "voxel/voxel.vert" ), FilePath( "voxel/voxel.geom" ), FilePath( "voxel/voxel.frag" ) } );

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
