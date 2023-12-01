#ifndef __VTX_RENDERER_GL_PASS_SELECTION__
#define __VTX_RENDERER_GL_PASS_SELECTION__

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/program.hpp"
#include "renderer/gl/texture_2d.hpp"
#include <util/color/rgba.hpp>

namespace VTX::Renderer::GL::Pass
{
	class Selection
	{
	  public:
		Selection( const size_t p_width, const size_t p_height, ProgramManager & p_pm );
		void resize( const size_t p_width, const size_t p_height );
		void render( VertexArray & p_vao );

		void setColor( const Util::Color::Rgba & );

		struct StructIn
		{
			Texture2D * textureDataPacked = nullptr;
			Texture2D * textureColor	  = nullptr;
			Texture2D * textureDepth	  = nullptr;
		} in;

		struct StructOut
		{
			std::unique_ptr<Framebuffer> fbo;
			std::unique_ptr<Texture2D>	 texture;
		} out;

		struct StructUniforms
		{
			Util::Color::Rgba color = COLOR_SELECTION_DEFAULT;
		} uniforms;

		inline static const Util::Color::Rgba COLOR_SELECTION_DEFAULT = COLOR_GREEN;

	  private:
		std::unique_ptr<Buffer> _ubo;
		Program *				_program = nullptr;
	};

	using PassSelection = BasePass<Selection>;
} // namespace VTX::Renderer::GL::Pass

#endif
