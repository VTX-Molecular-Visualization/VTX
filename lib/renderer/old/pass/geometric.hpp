#ifndef __VTX_RENDERER_GL_PASS_GEOMETRIC__
#define __VTX_RENDERER_GL_PASS_GEOMETRIC__

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL
{
	struct StructBufferMeshes;
	struct StructBufferMolecules;

	namespace Pass
	{
		class Geometric
		{
		  public:
			Geometric( const size_t p_width, const size_t p_height, ProgramManager & p_pm );
			void resize( const size_t p_width, const size_t p_height );
			void render( VertexArray & p_vao );

			Vec2i getPickedData( const uint p_x, const uint p_y );

			struct StructIn
			{
				StructBufferMeshes *	meshes	  = nullptr;
				StructBufferMolecules * molecules = nullptr;
			} in;

			struct StructOut
			{
				std::unique_ptr<Framebuffer> fbo;
				std::unique_ptr<Texture2D>	 textureDataPacked;
				std::unique_ptr<Texture2D>	 textureColors;
				std::unique_ptr<Texture2D>	 textureDepth;
				std::unique_ptr<Texture2D>	 texturePicking;
			} out;

		  private:
			Program * _programSphere   = nullptr;
			Program * _programCylinder = nullptr;
			Program * _programRibbon   = nullptr;
			Program * _programLine	   = nullptr;
			Program * _programTriangle = nullptr;
			Program * _programVoxel	   = nullptr;
		}; // namespace VTX::Pass

		using PassGeometric = BasePass<Geometric>;
	} // namespace Pass
} // namespace VTX::Renderer::GL

#endif
