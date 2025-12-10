#ifndef __VTX_RENDERER_DESCRIPTORS__
#define __VTX_RENDERER_DESCRIPTORS__

#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <util/types.hpp>
#include <variant>
#include <vector>

/**
 * @brief Describes all the meta-data used by the render graph.
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

	/**
	 * @brief Aliases.
	 */
	using Key	= std::string;
	using Keys	= std::vector<Key>;
	using Files = std::variant<FilePath, std::vector<FilePath>>;

	/**
	 * @brief Texture descriptor.
	 */
	struct Texture
	{
		E_FORMAT format;
	};

	/**
	 * @brief Vertex attribute descriptor.
	 */
	struct VertexAttribute
	{
		Key			 name;
		E_TYPE		 type;
		std::uint8_t components;
	};

	/**
	 * @brief Vertex layout descriptor.
	 */
	struct VertexLayout
	{
		std::vector<VertexAttribute> attributes;
	};

	/**
	 * @brief Uniform value descriptor.
	 */
	struct UniformValue
	{
		Key										 name;
		E_TYPE									 type;
		std::array<std::uint8_t, 64>			 data;
		std::optional<std::pair<double, double>> range;
	};

	/**
	 * @brief Uniform buffer descriptor.
	 */
	struct UniformBuffer
	{
		Key						  name;
		std::uint32_t			  binding;
		std::vector<UniformValue> values;
	};

	/**
	 * @brief All resources.
	 */
	struct Resources
	{
		std::unordered_map<Key, Texture>	   textures;
		std::unordered_map<Key, VertexLayout>  vertexStreams;
		std::unordered_map<Key, UniformBuffer> uniformBuffers;
	};

	/**
	 * @brief Draw call descriptor.
	 */
	struct DrawCall
	{
		Key			vertexStream;
		E_PRIMITIVE primitive  = E_PRIMITIVE::TRIANGLES;
		bool		useIndices = false;
	};

	/**
	 * @brief Program descriptor.
	 */
	struct Program
	{
		Key						  name;
		Files					  shaders;
		std::vector<UniformValue> uniforms;
		std::optional<DrawCall>	  drawCall;
	};

	/**
	 * @brief Render function descriptor.
	 */
	using RenderFunc = std::function<void()>;

	/**
	 * @brief Pass descriptor.
	 */
	struct Pass
	{
		Key						  name;
		Keys					  inputs;
		Keys					  outputs;
		std::vector<Program>	  programs;
		std::optional<RenderFunc> customCallback;
	};

	/**
	 * @brief Aliases.
	 */
	using PassList = std::vector<std::unique_ptr<Pass>>;

} // namespace VTX::Renderer

#endif
