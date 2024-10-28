#ifndef __VTX_RENDERER_ENUMS__
#define __VTX_RENDERER_ENUMS__

/**
 * @brief Describes all enums used by descriptors.
 */
namespace VTX::Renderer
{
	enum struct E_CHAN_IN
	{
		_0,
		_1,
		_2,
		_3,
		_4,
		_5,
		_6,
		_7,
	};

	enum struct E_CHAN_OUT
	{
		COLOR_0,
		COLOR_1,
		COLOR_2,
		DEPTH,
	};

	enum struct E_FORMAT
	{
		RGB16F,
		RGBA16F,
		RGBA32UI,
		RGBA32F,
		RG32UI,
		R8,
		R16F,
		R32F,
		DEPTH_COMPONENT32F
	};

	enum struct E_PRIMITIVE
	{
		POINTS,
		LINES,
		TRIANGLES,
		PATCHES,
	};

	enum struct E_TYPE
	{
		BOOL,
		BYTE,
		UBYTE,
		SHORT,
		USHORT,
		INT,
		UINT,
		FLOAT,
		VEC2I,
		VEC2F,
		VEC3F,
		VEC4F,
		MAT3F,
		MAT4F,
		COLOR4
	};

	enum struct E_ACCESS
	{
		READ,
		WRITE,
		READ_WRITE,
	};

	enum struct E_SETTING
	{
		CLEAR
	};

	enum struct E_WRAPPING
	{
		REPEAT,
		MIRRORED_REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER,
		MIRROR_CLAMP_TO_EDGE,
	};

	enum struct E_FILTERING
	{
		NEAREST,
		LINEAR,
		NEAREST_MIPMAP_NEAREST,
		LINEAR_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_LINEAR,
	};

} // namespace VTX::Renderer

#endif
