#ifndef __VTX_RENDERER_GL_PASS_GEOMETRIC__
#define __VTX_RENDERER_GL_PASS_GEOMETRIC__

#include "base_pass.hpp"
#include "renderer/gl/buffer.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Geometric : public BasePass
	{
	  public:
		Geometric()			 = default;
		virtual ~Geometric() = default;

		void init( const size_t p_width, const size_t p_height, ProgramManager & p_pm ) override;
		void resize( const size_t p_width, const size_t p_height ) override;
		void render( VertexArray & p_vao ) override;

		Vec2i getPickedData( const uint p_x, const uint p_y );

		struct StructIn
		{
			struct Triangle
			{
				Buffer		vboPositions	= Buffer();
				Buffer		vboNormals		= Buffer();
				Buffer		vboColors		= Buffer();
				Buffer		vboVisibilities = Buffer();
				Buffer		vboSelections	= Buffer();
				Buffer		vboIds			= Buffer();
				Buffer		ibo				= Buffer();
				VertexArray vao				= VertexArray();
			} triangles;
		} in;

		struct StructOut
		{
			Framebuffer fbo							= Framebuffer();
			Texture2D	textureViewPositionsNormals = Texture2D();
			Texture2D	textureColors				= Texture2D();
			Texture2D	textureDepth				= Texture2D();
			Texture2D	texturePicking				= Texture2D();
		} out;

	  private:
		Program * _programSphere   = nullptr;
		Program * _programCylinder = nullptr;
		Program * _programRibbon   = nullptr;
		Program * _programLine	   = nullptr;
		Program * _programTriangle = nullptr;
		Program * _programVoxel	   = nullptr;
	}; // namespace VTX::Pass
} // namespace VTX::Renderer::GL::Pass

#endif
