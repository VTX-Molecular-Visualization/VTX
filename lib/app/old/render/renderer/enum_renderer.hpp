#ifndef __VTX_APP_OLD_RENDER_RENDERER_ENUM_RENDERER__
#define __VTX_APP_OLD_RENDER_RENDERER_ENUM_RENDERER__

namespace VTX::App::Old::Render::Renderer
{
	enum class MODE : int
	{
		GL,
		RT_CPU,
		COUNT
	};

	enum class SHADING : int
	{
		DIFFUSE = 0,
		GLOSSY,
		TOON,
		FLAT_COLOR,
		COUNT
	};

	enum class RENDER_SETTING : int
	{
		SHADING = 0,
		SSAO,
		OUTLINE,
		FOG,
		AA
	};
} // namespace VTX::App::Old::Render::Renderer

#endif
