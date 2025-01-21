#ifndef __VTX_RENDERER_ENUMS__
#define __VTX_RENDERER_ENUMS__

/**
 * @brief Describes all enums used by descriptors.
 */
namespace VTX::Renderer
{
	/**
	 * @brief All graphic API implementations.
	 */
	enum struct E_GRAPHIC_API
	{
		DEFAULT,
		OPENGL45
	};

	/**
	 * @brief All shading modes.
	 */
	enum struct E_SHADING
	{
		DIFFUSE,
		GLOSSY,
		TOON,
		FLAT,
		COUNT
	};

	/**
	 * @brief All input channels.
	 */
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

	/**
	 * @brief All output channels.
	 */
	enum struct E_CHAN_OUT
	{
		COLOR_0,
		COLOR_1,
		COLOR_2,
		DEPTH,
	};

	/**
	 * @brief All data formats.
	 */
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

	/**
	 * @brief All draw primitives.
	 */
	enum struct E_PRIMITIVE
	{
		POINTS,
		LINES,
		TRIANGLES,
		PATCHES,
	};

	/**
	 * @brief All data types.
	 */
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

	/**
	 * @brief All access types.
	 */
	enum struct E_ACCESS
	{
		READ,
		WRITE,
		READ_WRITE,
	};

	/**
	 * @brief All usages.
	 */
	enum struct E_MUTABLE_USAGE
	{
		STATIC_DRAW,
		STATIC_READ,
		STATIC_COPY,
		DYNAMIC_DRAW,
		DYNAMIC_READ,
		DYNAMIC_COPY,
		// STREAM_DRAW,
		// STREAM_READ,
		// STREAM_COPY,
	};

	/*
	enum struct E_TARGET
	{
		UNIFORM_BUFFER,
		SHADER_STORAGE_BUFFER
	};
	*/

	/**
	 * @brief All immutable flags.
	 */
	enum struct E_IMMUTABLE_FLAG
	{
		DYNAMIC_STORAGE_BIT, // Allors update subdata?
		MAP_READ,			 // Allows mapping for reading.
		MAP_WRITE,			 // Allows mapping for writing.

		// TODO: check.
		// MAP_PERSISTENT,
		// MAP_COHERENT,
		// CLIENT_STORAGE_BIT
	};

	/**
	 * @brief All pass settings.
	 */
	enum struct E_SETTING
	{
		CLEAR
	};

	/**
	 * @brief All texture wrapping modes.
	 */
	enum struct E_WRAPPING
	{
		REPEAT,
		MIRRORED_REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER,
		MIRROR_CLAMP_TO_EDGE,
	};

	/**
	 * @brief All texture filtering modes.
	 */
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
