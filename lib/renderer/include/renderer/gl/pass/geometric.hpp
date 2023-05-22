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
		Geometric( const size_t p_width, const size_t p_height, ProgramManager & p_pm );
		virtual ~Geometric() = default;

		void resize( const size_t p_width, const size_t p_height ) override;
		void render( VertexArray & p_vao ) override;

		Vec2i getPickedData( const uint p_x, const uint p_y );

		struct StructIn
		{
			struct Triangle
			{
				Buffer		vboPositions;
				Buffer		vboNormals;
				Buffer		vboColors;
				Buffer		vboVisibilities;
				Buffer		vboSelections;
				Buffer		vboIds;
				Buffer		ibo;
				VertexArray vao;
			} triangles;
		} in;

		struct StructOut
		{
			Framebuffer fbo;
			Texture2D	textureViewPositionsNormals;
			Texture2D	textureColors;
			Texture2D	textureDepth;
			Texture2D	texturePicking;
		} out;

	  private:
		Program * _programSphere;
		Program * _programCylinder;
		Program * _programRibbon;
		Program * _programLine;
		Program * _programTriangle;
		Program * _programVoxel;
	}; // namespace VTX::Pass
} // namespace VTX::Renderer::GL::Pass

#endif
