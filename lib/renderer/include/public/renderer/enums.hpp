#ifndef __VTX_RENDERER_ENUMS__
#define __VTX_RENDERER_ENUMS__

#include <cstdint>

/**
 * @brief Describes all enums used by descriptors.
 */
namespace VTX::Renderer
{
	/**
	 * @brief All data types.
	 */
	enum struct E_TYPE : std::uint8_t
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
	 * @brief Global resource types.
	 */
	enum struct E_RESOURCE_TYPE : std::uint8_t
	{
		TEXTURE,
		VERTEX_STREAM,
		UNIFORM_BUFFER
	};

	/**
	 * @brief All data formats.
	 */
	enum struct E_FORMAT : std::uint8_t
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
	enum struct E_PRIMITIVE : std::uint8_t
	{
		POINTS,
		LINES,
		TRIANGLES,
		PATCHES,
	};

} // namespace VTX::Renderer

#endif
