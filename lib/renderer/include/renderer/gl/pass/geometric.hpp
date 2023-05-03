#ifndef __VTX_RENDERER_GL_PASS_GEOMETRIC__
#define __VTX_RENDERER_GL_PASS_GEOMETRIC__

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/texture_2d.hpp"

namespace VTX::Renderer::GL::Pass
{
	class Geometric : public BasePass
	{
	  public:
		Geometric()			 = default;
		virtual ~Geometric() = default;

		void init( const size_t p_width, const size_t p_height ) override;
		void resize( const size_t p_width, const size_t p_height ) override;
		void render() override;

		struct StructIn
		{
			// TODO.
		} in;

		struct StructOut
		{
			Framebuffer fbo							= Framebuffer();
			Texture2D	textureViewPositionsNormals = Texture2D();
			Texture2D	textureColors				= Texture2D();
			Texture2D	textureDepth				= Texture2D();
			Texture2D	texturePicking				= Texture2D();
		} out;
	}; // namespace VTX::Renderer::GL::Pass
} // namespace VTX::Renderer::GL::Pass

#endif
