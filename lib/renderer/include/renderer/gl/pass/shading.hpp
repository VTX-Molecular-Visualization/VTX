#ifndef __VTX_RENDERER_GL_PASS_SHADING__
#define __VTX_RENDERER_GL_PASS_SHADING__

#include "base_pass.hpp"
#include "renderer/gl/enum_shading.hpp"
#include "renderer/gl/framebuffer.hpp"
#include "renderer/gl/program.hpp"
#include "renderer/gl/texture_2d.hpp"
#include <util/color/rgba.hpp>

namespace VTX::Renderer::GL::Pass
{
	class Shading
	{
	  public:
		Shading( const size_t p_width, const size_t p_height, ProgramManager & p_pm );
		void resize( const size_t p_width, const size_t p_height );
		void render( VertexArray & p_vao );

		void setMode( const ENUM_SHADING );
		void setSpecularFactor( const float );
		void setColorBackground( const Util::Color::Rgba & );
		void setColorLight( const Util::Color::Rgba & );
		void setColorFog( const Util::Color::Rgba & );
		void setFogNear( const float );
		void setFogFar( const float );
		void setFogDensity( const float );

		struct StructIn
		{
			Texture2D * textureDataPacked = nullptr;
			Texture2D * textureColor	  = nullptr;
			Texture2D * textureBlur		  = nullptr;
		} in;

		struct StructOut
		{
			std::unique_ptr<Framebuffer> fbo;
			std::unique_ptr<Texture2D>	 texture;

		} out;

		struct StructUniforms
		{
			Util::Color::Rgba colorBackground = Util::Color::Rgba::BLACK;
			Util::Color::Rgba colorLight	  = Util::Color::Rgba::WHITE;
			Util::Color::Rgba colorFog		  = Util::Color::Rgba::WHITE;
			int32_t			  shadingMode	  = ENUM_SHADING::DIFFUSE;
			float			  specularFactor  = 0.4f;
			float			  fogNear		  = 30.f;
			float			  fogFar		  = 80.f;
			float			  fogDensity	  = 0.f;
		} uniforms;

	  private:
		std::unique_ptr<Buffer> _ubo;
		Program *				_program = nullptr;
	};

	using PassShading = BasePass<Shading>;
} // namespace VTX::Renderer::GL::Pass

#endif
