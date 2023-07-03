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
			Util::Color::Rgba colorBackground = COLOR_BACKGROUND_DEFAULT;
			Util::Color::Rgba colorLight	  = COLOR_LIGHT_DEFAULT;
			Util::Color::Rgba colorFog		  = COLOR_FOG_DEFAULT;
			int32_t			  shadingMode	  = SHADING_MODE_DEFAULT;
			float			  specularFactor  = SPECULAR_FACTOR_DEFAULT;
			float			  fogNear		  = FOG_NEAR_DEFAULT;
			float			  fogFar		  = FOG_FAR_DEFAULT;
			float			  fogDensity	  = FOG_DENSITY_DEFAULT;
		} uniforms;

		inline static const Util::Color::Rgba COLOR_BACKGROUND_DEFAULT = COLOR_BLACK;
		inline static const Util::Color::Rgba COLOR_LIGHT_DEFAULT	   = COLOR_WHITE;
		inline static const Util::Color::Rgba COLOR_FOG_DEFAULT		   = COLOR_WHITE;
		inline static const ENUM_SHADING	  SHADING_MODE_DEFAULT	   = ENUM_SHADING::DIFFUSE;
		inline static const float			  SPECULAR_FACTOR_DEFAULT  = 0.4f;
		inline static const float			  SPECULAR_FACTOR_MIN	   = 0.f;
		inline static const float			  SPECULAR_FACTOR_MAX	   = 1.f;
		inline static const float			  FOG_NEAR_DEFAULT		   = 30;
		inline static const float			  FOG_NEAR_MIN			   = 0.f;
		inline static const float			  FOG_NEAR_MAX			   = 1000.f;
		inline static const float			  FOG_FAR_DEFAULT		   = 80.f;
		inline static const float			  FOG_FAR_MIN			   = 0.f;
		inline static const float			  FOG_FAR_MAX			   = 1000.f;
		inline static const float			  FOG_DENSITY_DEFAULT	   = 0.f;
		inline static const float			  FOG_DENSITY_MIN		   = 0.f;
		inline static const float			  FOG_DENSITY_MAX		   = 1.f;

	  private:
		std::unique_ptr<Buffer> _ubo;
		Program *				_program = nullptr;
	};

	using PassShading = BasePass<Shading>;
} // namespace VTX::Renderer::GL::Pass

#endif
