#ifndef __VTX_RENDERER_GL_PASS_OUTLINE__
#define __VTX_RENDERER_GL_PASS_OUTLINE__

#include "base_pass.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/program.hpp"
#include "renderer/gl/texture_2d.hpp"
#include <util/color/rgba.hpp>

namespace VTX::Renderer::GL::Pass
{
	class Outline
	{
	  public:
		Outline( const size_t p_width, const size_t p_height, ProgramManager & p_pm );
		void resize( const size_t p_width, const size_t p_height );
		void render( VertexArray & p_vao );

		void setSensivity( const float );
		void setThickness( const float );
		void setColor( const Util::Color::Rgba & );

		struct StructIn
		{
			Texture2D * textureColor = nullptr;
			Texture2D * textureDepth = nullptr;
		} in;

		struct StructOut
		{
			std::unique_ptr<Framebuffer> fbo;
			std::unique_ptr<Texture2D>	 texture;
		} out;

		struct StructUniforms
		{
			float			  sensivity = 0.4f;
			float			  thickness = 1.f;
			Util::Color::Rgba color		= Util::Color::Rgba::WHITE;
		} uniforms;

	  private:
		std::unique_ptr<Buffer> _ubo;
		Program *				_program = nullptr;
	};

	using PassOutline = BasePass<Outline>;
} // namespace VTX::Renderer::GL::Pass

#endif
