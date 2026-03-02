#ifndef __VTX_RENDERER_DESCRIPTORS__
#define __VTX_RENDERER_DESCRIPTORS__

#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <tuple>
#include <unordered_map>
#include <util/constants.hpp>
#include <util/hashing.hpp>
#include <util/types.hpp>
#include <variant>
#include <vector>

/**
 * @brief Describes all the meta-data used by the render graph.
 */
namespace VTX::Renderer::Desc
{
	/**
	 * @brief All data types.
	 */
	enum struct E_TYPE : uint32_t
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
		MAT4F
	};

	/**
	 * @brief Pass resource types.
	 */
	enum struct E_RESOURCE_TYPE : uint32_t
	{
		TEXTURE,
		GEOMETRY,
		BUFFER
	};

	/**
	 * @brief All data formats.
	 */
	enum struct E_FORMAT : uint32_t
	{
		RGBA8UI,
		RGB16F,
		RGBA16F,
		RGBA32UI,
		RGBA32F,
		RG32UI,
		R8UI,
		R16F,
		R32F,
		DEPTH_COMPONENT32F
	};

	/**
	 * @brief All draw primitives.
	 */
	enum struct E_PRIMITIVE : uint32_t
	{
		POINTS,
		LINES,
		TRIANGLES,
		PATCHES,
	};

	/**
	 * @brief All buffer roles.
	 */
	enum struct E_SHADER_BUFFER_KIND : uint32_t
	{
		PARAMETERS,
		STRUCTURED
	};

	/**
	 * @brief All buffer mutability types.
	 */
	enum struct E_BUFFER_MUTABILITY : uint32_t
	{
		MUTABLE,
		IMMUTABLE
	};

	/**
	 * @brief All buffer access types.
	 */
	enum struct E_BUFFER_ACCESS : uint32_t
	{
		NONE,
		READ,
		WRITE,
		READ_WRITE
	};

	/**
	 * @brief All buffer update frequencies.
	 */
	enum struct E_UPDATE_FREQUENCY : uint32_t
	{
		STATIC,
		DYNAMIC,
		STREAM
	};

	/**
	 * @brief All data buffer kinds.
	 */
	enum struct E_PIPELINE_BUFFER_KIND : uint32_t
	{
		VERTEX,
		INDEX,
		INDIRECT_COMMAND
	};

	/**
	 * @brief All sampler wrapping modes.
	 */
	enum struct E_WRAPPING : uint32_t
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
	enum struct E_FILTERING : uint32_t
	{
		NEAREST,
		LINEAR,
		NEAREST_MIPMAP_NEAREST,
		LINEAR_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_LINEAR,
	};

	/**
	 * @brief Various settings.
	 */
	enum struct E_SETTING : uint32_t
	{
		CLEAR_COLOR	 = 1u << 0,
		CLEAR_DEPTH	 = 1u << 1,
		ENABLE_DEPTH = 1u << 2,
	};

	/**
	 * @brief All render target types.
	 */
	enum struct E_RENDER_TARGET : uint32_t
	{
		SCREEN,
		OFFSCREEN
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
	using Files		 = std::vector<FilePath>;
	using DirOrFiles = std::variant<FilePath, Files>;

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
		E_WRAPPING	wrapS;
		E_WRAPPING	wrapT;
		E_FILTERING minFilter;
		E_FILTERING magFilter;

		/*
		float lodBias = 0.f;
		float minLod  = -1000.f;
		float maxLod  = 1000.f;

		float anisotropy = 1.f;
		*/
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
		std::optional<uint32_t>					 arrayCount;
	};

	/**
	 * @brief Uniform buffer descriptor.
	 */
	struct BufferShader
	{
		Key						  name;
		E_SHADER_BUFFER_KIND	  role;
		E_BUFFER_MUTABILITY		  mutability;
		E_BUFFER_ACCESS			  access;
		E_UPDATE_FREQUENCY		  frequency;
		std::optional<Binding>	  binding; // TODO: remove and use backend reflection.
		std::vector<UniformValue> values;
	};

	/**
	 * @brief Data buffer descriptor.
	 */
	struct BufferPipeline
	{
		Key					   name;
		E_PIPELINE_BUFFER_KIND kind;
		E_UPDATE_FREQUENCY	   frequency;
	};

	/**
	 * @brief Resource binding descriptor.
	 */
	struct ResourceBinding
	{
		E_RESOURCE_TYPE	   type;
		Key				   primary;
		std::optional<Key> secondary;
	};

	/**
	 * @brief Vertex attribute descriptor.
	 */
	struct VertexAttribute
	{
		Key	   name;
		E_TYPE type;
	};

	/**
	 * @brief Vertex layout descriptor.
	 */
	struct VertexLayout
	{
		std::vector<VertexAttribute> attributes;
	};

	/**
	 * @brief Geometry descriptor.
	 */
	struct Geometry
	{
		Key				   vertexLayout;
		std::optional<Key> indexBuffer;
		std::optional<Key> indirectBuffer;
		// std::unordered_map<Key, Key> overrides; // attributeName -> bufferKey
	};

	/**
	 * @brief Indirect draw command descriptors.
	 */
	struct DrawIndirectCommand
	{
		uint32_t vertexCount;
		uint32_t instanceCout;
		uint32_t firstVertex;
		uint32_t baseInstance;
	};

	struct DrawIndexedIndirectCommand
	{
		uint32_t indexCount;
		uint32_t instanceCount;
		uint32_t firstIndex;
		int32_t	 baseVertex;
		uint32_t baseInstance;
	};

	/**
	 * @brief Draw call descriptor.
	 */
	struct DrawCall
	{
		Key			geometry;
		E_PRIMITIVE primitive;

		struct Range
		{
			uint32_t first;
			uint32_t count;
		};
		// Could be a single range, or reference to a count in a buffer for indirect draw.
		using RangesVariant = std::variant<Range, std::reference_wrapper<const uint32_t>>;
		RangesVariant ranges;
	};

	/**
	 * @brief Program descriptor.
	 */
	struct Program
	{
		Key						  name;
		DirOrFiles				  shaders;
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
		std::vector<E_SETTING>		 settings;
		std::optional<RenderFunc>	 customCallback;
	};

	/**
	 * @brief All resources.
	 */
	template<typename T>
	using ResourceMap = std::unordered_map<Key, T>;
	struct Resources
	{
		ResourceMap<Texture>		textures;
		ResourceMap<Sampler>		samplers;
		ResourceMap<VertexLayout>	vertexStreams;
		ResourceMap<BufferShader>	shaderBuffers;
		ResourceMap<BufferPipeline> pipelineBuffers;
		ResourceMap<Geometry>		geometries;
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

	/**
	 * @brief Descriptor hash functions.
	 */
	template<typename T>
	Hash hashDesc( const T & )
	{
		return 0;
	}

	template<>
	inline Hash hashDesc<Texture>( const Texture & p_text )
	{
		return Util::hash( toUnderlying( p_text.format ) ) + p_text.data.size();
	}

} // namespace VTX::Renderer::Desc

#endif
