#ifndef __VTX_RENDERER_DESCRIPTORS__
#define __VTX_RENDERER_DESCRIPTORS__

#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <util/constants.hpp>
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
	enum struct E_TYPE : uint8_t
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
	enum struct E_RESOURCE_TYPE : uint8_t
	{
		TEXTURE,
		VERTEX_STREAM,
		BUFFER
	};

	/**
	 * @brief All data formats.
	 */
	enum struct E_FORMAT : uint8_t
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
	enum struct E_PRIMITIVE : uint8_t
	{
		POINTS,
		LINES,
		TRIANGLES,
		PATCHES,
	};

	/**
	 * @brief All buffer roles.
	 */
	enum struct E_BUFFER_ROLE : uint8_t
	{
		UNIFORM,
		STORAGE
	};

	enum class E_BUFFER_ACCESS : uint8_t
	{
		READ,
		READ_WRITE
	};

	enum class E_BUFFER_CLASS : uint8_t
	{
		UNIFORM_LIKE,
		STRUCTURED
	};

	/**
	 * @brief All buffer update frequencies.
	 */
	enum class E_UPDATE_FREQUENCY : uint8_t
	{
		PER_FRAME,
		PER_PASS,
		PER_DRAW,
		STATIC
	};

	/**
	 * @brief Aliases.
	 */
	using Key	= std::string;
	using Keys	= std::vector<Key>;
	using Files = std::vector<FilePath>;

	/**
	 * @brief Texture descriptor.
	 */
	struct Texture
	{
		E_FORMAT			 format;
		std::vector<uint8_t> data;
	};

	/**
	 * @brief Vertex attribute descriptor.
	 */
	struct VertexAttribute
	{
		Key		name;
		E_TYPE	type;
		uint8_t components;
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
		std::array<uint8_t, 64>					 data;
		std::optional<std::pair<double, double>> range;
		uint32_t								 arrayCount;
	};

	/**
	 * @brief Uniform buffer descriptor.
	 */
	struct BufferLayout
	{
		Key						  name;
		E_BUFFER_CLASS			  dataClass;
		E_BUFFER_ACCESS			  access;
		E_UPDATE_FREQUENCY		  frequency;
		std::uint32_t			  binding; // TODO: remove and use backend reflection.
		std::vector<UniformValue> values;
	};

	/**
	 * @brief All resources.
	 */
	struct Resources
	{
		std::unordered_map<Key, Texture>	  textures;
		std::unordered_map<Key, VertexLayout> vertexStreams;
		std::unordered_map<Key, BufferLayout> buffers;
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
	using PassList			   = std::vector<std::unique_ptr<Pass>>;
	using Handle			   = uint32_t;
	constexpr Handle NO_HANDLE = TypeMax<Handle>;

	/**
	 * @brief Ordered list of passes for execution.
	 */
	using RenderQueue = std::vector<const Pass *>;

} // namespace VTX::Renderer

#endif
