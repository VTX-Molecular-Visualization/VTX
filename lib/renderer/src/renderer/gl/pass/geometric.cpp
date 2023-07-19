#include "renderer/gl/pass/geometric.hpp"
#include "renderer/gl/struct_buffer_meshes.hpp"
#include "renderer/gl/struct_buffer_molecules.hpp"
#include <util/color/rgba.hpp>
#include <util/logger.hpp>

namespace VTX::Renderer::GL::Pass
{
	Geometric::Geometric( const size_t p_width, const size_t p_height, ProgramManager & p_pm )
	{
		out.fbo				  = std::make_unique<Framebuffer>();
		out.textureDataPacked = std::make_unique<Texture2D>(
			p_width, p_height, GL_RGBA32UI, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		out.textureColors = std::make_unique<Texture2D>(
			p_width, p_height, GL_RGBA16F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		out.textureDepth = std::make_unique<Texture2D>(
			p_width, p_height, GL_DEPTH_COMPONENT32F, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		out.texturePicking = std::make_unique<Texture2D>(
			p_width, p_height, GL_RG32UI, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST );

		out.fbo->attachTexture( *out.textureDataPacked, GL_COLOR_ATTACHMENT0 );
		out.fbo->attachTexture( *out.textureColors, GL_COLOR_ATTACHMENT1 );
		out.fbo->attachTexture( *out.textureDepth, GL_DEPTH_ATTACHMENT );
		out.fbo->attachTexture( *out.texturePicking, GL_COLOR_ATTACHMENT2 );

		out.fbo->setDrawBuffers( { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 } );

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
		out.textureDataPacked->resize( p_width, p_height );
		out.textureColors->resize( p_width, p_height );
		out.textureDepth->resize( p_width, p_height );
		out.texturePicking->resize( p_width, p_height );

		out.fbo->attachTexture( *out.textureDataPacked, GL_COLOR_ATTACHMENT0 );
		out.fbo->attachTexture( *out.textureColors, GL_COLOR_ATTACHMENT1 );
		out.fbo->attachTexture( *out.textureDepth, GL_DEPTH_ATTACHMENT );
		out.fbo->attachTexture( *out.texturePicking, GL_COLOR_ATTACHMENT2 );
	}

	void Geometric::render( VertexArray & p_vao )
	{
		assert( in.meshes != nullptr );
		assert( in.molecules != nullptr );

		glEnable( GL_DEPTH_TEST );
		glDepthFunc( GL_LESS );
		out.fbo->bind( GL_DRAW_FRAMEBUFFER );
		out.fbo->clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// Triangles.
		if ( in.meshes->size > 0 )
		{
			_programTriangle->use();
			in.meshes->vao.drawElement( GL_TRIANGLES, GLsizei( in.meshes->size ), GL_UNSIGNED_INT );
		}
		// Spheres.
		if ( in.molecules->sizeAtoms > 0 )
		{
			_programSphere->use();
			in.molecules->vao.drawArray( GL_POINTS, 0, GLsizei( in.molecules->sizeAtoms ) );
		}
		// Cylinders.
		in.molecules->bindBonds();
		if ( in.molecules->sizeBonds > 0 )
		{
			_programCylinder->use();
			in.molecules->vao.drawElement( GL_LINES, GLsizei( in.molecules->sizeBonds ), GL_UNSIGNED_INT );
		}
		in.molecules->vao.unbindElementBuffer();
		// TODO: Ribbons.

		out.fbo->unbind();
		glDisable( GL_DEPTH_TEST );
	}

	Vec2i Geometric::getPickedData( const uint p_x, const uint p_y )
	{
		out.fbo->bind( GL_READ_FRAMEBUFFER );
		out.fbo->setReadBuffer( GL_COLOR_ATTACHMENT2 );
		Vec2i data = Vec2i( INVALID_MODEL_ID, INVALID_MODEL_ID );
		glReadPixels( p_x, p_y, 1, 1, GL_RG_INTEGER, GL_UNSIGNED_INT, &data );
		out.fbo->unbind();
		return data;
	}

} // namespace VTX::Renderer::GL::Pass
