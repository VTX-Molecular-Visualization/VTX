#ifndef __VTX_RENDERER_ENUMS__
#define __VTX_RENDERER_ENUMS__

namespace VTX::Renderer
{

	enum struct E_PASS_TYPE
	{
		// GEOMETRIC,
		GRAPHIC,
		// COMPUTE,
		// ...?
	};

	enum struct E_CHANNEL_INPUT
	{
		_0,
		_1,
		_2,
		_3,
	};

	enum struct E_CHANNEL_OUTPUT
	{
		COLOR_0,
		COLOR_1,
		COLOR_2,
		DEPTH,
	};

	enum struct E_FORMAT
	{
		RGBA16F,
		RGBA32UI,
		RGBA32F,
		RG32UI,
		R16F,
		R32F,
		DEPTH_COMPONENT32F
	};

	enum struct E_PRIMTIVE
	{
		POINTS,
		LINES,
		TRIANGLES
	};

	enum struct E_DRAW_TYPE
	{

	};

	/*
	enum struct E_HANDLE_TYPE
	{
		ATTACHMENT,
		STORAGE,
		// UNIFORM,
		PROGRAM,
		// ...?
	};
	*/

	enum struct E_TYPE
	{
		UINT,
		INT,
		FLOAT,
		VEC3F,
		VEC4F,
		MAT3F,
		MAT4F,
	};

	enum struct E_ACCESS
	{
		READ,
		WRITE,
		READ_WRITE,
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
