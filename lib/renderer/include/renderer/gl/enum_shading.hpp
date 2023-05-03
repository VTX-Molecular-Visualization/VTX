#ifndef __VTX_RENDERER_GL_ENUM_SHADING__
#define __VTX_RENDERER_GL_ENUM_SHADING__

namespace VTX::Renderer::GL
{
	enum class ENUM_SHADING : unsigned int
	{
		DIFFUSE,
		GLOSSY,
		TOON,
		FLAT_COLOR
	};
}

#endif