#ifndef __VTX_RENDERER_DESCRIPTORS__
#define __VTX_RENDERER_DESCRIPTORS__

#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
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

	/**
	 * @brief All buffer access types.
	 */
	enum class E_BUFFER_ACCESS : uint8_t
	{
		READ,
		READ_WRITE
	};

	/**
	 * @brief All buffer data classes.
	 */
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
	 * @brief All sampler wrapping modes.
	 */
	enum struct E_WRAPPING : uint8_t
	{
		REPEAT,
		MIRRORED_REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER,
		MIRROR_CLAMP_TO_EDGE,
	};

	/**
	 * @brief All sampler filtering modes.
	 */
	enum struct E_FILTERING : uint8_t
	{
		NEAREST,
		LINEAR,
		NEAREST_MIPMAP_NEAREST,
		LINEAR_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_LINEAR,
	};

	/**
	 * @brief Aliases.
	 */
	using Key = std::string;

	/**
	 * @brief Binding slot.
	 */
	using Binding = uint32_t;

	/**
	 * @brief Files.
	 */
	using Files = std::vector<FilePath>;

	/**
	 * @brief 2D size absolute.
	 */
	struct Size2DAbsolute
	{
		uint32_t width	= 0;
		uint32_t height = 0;
	};

	/**
	 * @brief 2D size relative.
	 */
	struct Size2DRelative
	{
		float width	 = 0.f;
		float height = 0.f;
	};

	/**
	 * @brief 2D size variant.
	 * monostate == auto
	 */
	using Size2D = std::variant<std::monostate, Size2DAbsolute, Size2DRelative>;

	/**
	 * @brief Texture descriptor.
	 */
	struct Texture
	{
		E_FORMAT			   format;
		Size2D				   size;
		std::vector<std::byte> data;
	};

	struct Sampler
	{
		E_WRAPPING	wrapS	  = E_WRAPPING::CLAMP_TO_EDGE;
		E_WRAPPING	wrapT	  = E_WRAPPING::CLAMP_TO_EDGE;
		E_FILTERING minFilter = E_FILTERING::NEAREST;
		E_FILTERING magFilter = E_FILTERING::NEAREST;

		/*
		float lodBias = 0.f;
		float minLod  = -1000.f;
		float maxLod  = 1000.f;

		float anisotropy = 1.f;
		*/
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
		std::optional<uint32_t>					 arrayCount = std::nullopt;
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
		Binding					  binding; // TODO: remove and use backend reflection.
		std::vector<UniformValue> values;
	};

	/**
	 * @brief Resource binding descriptor.
	 */
	struct ResourceBinding
	{
		Key				   primary;
		std::optional<Key> secondary; // for textures
	};

	/**
	 * @brief All resources.
	 */
	struct Resources
	{
		std::unordered_map<Key, Texture>	  textures;
		std::unordered_map<Key, Sampler>	  samplers;
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
		Key							 name;
		std::vector<ResourceBinding> inputs;
		std::vector<ResourceBinding> outputs;
		std::vector<Program>		 programs;
		std::optional<RenderFunc>	 customCallback;
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
