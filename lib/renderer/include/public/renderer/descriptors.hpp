#ifndef __VTX_RENDERER_DESCRIPTORS__
#define __VTX_RENDERER_DESCRIPTORS__

#include <array>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <tuple>
#include <unordered_map>
#include <util/constants.hpp>
#include <util/enum.hpp>
#include <util/hashing.hpp>
#include <util/types.hpp>
#include <utility>
#include <variant>
#include <vector>

/**
 * @brief Describes all the meta-data used by the render graph.
 */
namespace VTX::Renderer::Desc
{
	/**
	 * @brief Native context information.
	 */
	enum class E_NATIVE_PLATEFORM : uint8_t
	{
		UNKNOWN = 0,
		WINDOWS,
		X11,
		WAYLAND
	};

	/**
	 * @brief Native context information.
	 */
	struct NativeContextInfo
	{
		uintptr_t		   surface	 = 0;
		uintptr_t		   display	 = 0;
		E_NATIVE_PLATEFORM plateform = E_NATIVE_PLATEFORM::UNKNOWN;
	};

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
		VEC2U,
		VEC2F,
		VEC3F,
		VEC4F,
		VEC4U,
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
	 * @brief All ways a buffer can be consumed by the renderer or external compute backends.
	 */
	enum struct E_BUFFER_USAGE : uint32_t
	{
		VTX_ENUM_ENABLE_BITMASK,
		NONE	   = 0u,
		VERTEX	   = 1u << 0,
		INDEX	   = 1u << 1,
		INDIRECT   = 1u << 2,
		UNIFORM	   = 1u << 3,
		STORAGE	   = 1u << 4,
		CUDA_READ  = 1u << 5,
		CUDA_WRITE = 1u << 6,
	};

	/**
	 * @brief Physical allocation strategy for a logical render graph buffer.
	 */
	enum struct E_BUFFER_ALLOCATION : uint32_t
	{
		SINGLE,
		CHUNKED
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
		VTX_ENUM_ENABLE_BITMASK,
		CLEAR_COLOR	 = 1u << 0,
		CLEAR_DEPTH	 = 1u << 1,
		ENABLE_DEPTH = 1u << 2,
	};

	/**
	 * @brief Memory barriers required after a compute dispatch.
	 */
	enum struct E_MEMORY_BARRIER : uint32_t
	{
		VTX_ENUM_ENABLE_BITMASK,
		NONE				 = 0u,
		VERTEX_ATTRIB_ARRAY	 = 1u << 0,
		ELEMENT_ARRAY		 = 1u << 1,
		UNIFORM				 = 1u << 2,
		TEXTURE_FETCH		 = 1u << 3,
		SHADER_IMAGE_ACCESS	 = 1u << 4,
		COMMAND				 = 1u << 5,
		PIXEL_BUFFER		 = 1u << 6,
		TEXTURE_UPDATE		 = 1u << 7,
		BUFFER_UPDATE		 = 1u << 8,
		FRAMEBUFFER			 = 1u << 9,
		TRANSFORM_FEEDBACK	 = 1u << 10,
		ATOMIC_COUNTER		 = 1u << 11,
		SHADER_STORAGE		 = 1u << 12,
		QUERY				 = 1u << 13,
		CLIENT_MAPPED_BUFFER = 1u << 14,
		ALL					 = 0xFFFFFFFFu
	};

	/**
	 * @brief External compute APIs that can interop with renderer-owned resources.
	 */
	enum struct E_INTEROP_API : uint32_t
	{
		CUDA,
		OPTIX
	};

	/**
	 * @brief All pass types.
	 */
	enum struct E_PASS_TYPE : uint8_t
	{
		// Produces render commands for the graphics pipeline.
		GRAPHICS,
		// Produces dispatch commands for compute pipelines.
		COMPUTE,
		// Others: CUDA, etc...
		EXTERNAL
	};

	/**
	 * @brief All pass execution types.
	 */
	enum struct E_PASS_EXECUTION : uint8_t
	{
		// Execute every frame.
		EVERY_FRAME,
		// Execute only when explicitly marked dirty by the renderer.
		ON_DIRTY
	};

	/**
	 * @brief Set render target for the graphics pipeline.
	 */
	enum struct E_RENDER_TARGET : uint32_t
	{
		SCREEN,
		OFFSCREEN
	};

	/**
	 * @brief All options.
	 */
	enum struct E_OPTION : uint32_t
	{
		VSYNC
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
	 * @brief Buffer descriptor.
	 */
	struct Buffer
	{
		Key						  name;
		E_BUFFER_USAGE			  usage		 = E_BUFFER_USAGE::NONE;
		E_BUFFER_ALLOCATION		  allocation = E_BUFFER_ALLOCATION::SINGLE;
		E_BUFFER_MUTABILITY		  mutability = E_BUFFER_MUTABILITY::MUTABLE;
		E_BUFFER_ACCESS			  access	 = E_BUFFER_ACCESS::NONE;
		E_UPDATE_FREQUENCY		  frequency	 = E_UPDATE_FREQUENCY::STATIC;
		std::optional<Binding>	  binding	 = std::nullopt; // TODO: remove and use backend reflection?
		std::vector<UniformValue> values;
	};

	/**
	 * @brief Physical chunk of a buffer, used when allocation strategy is chunked.
	 */
	using BufferChunk = uint32_t;

	/**
	 * @brief Reference to a logical buffer, optionally targeting a physical chunk.
	 */
	struct BufferRef
	{
		Key						   key;
		std::optional<BufferChunk> chunk = std::nullopt;
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
		Key						 vertexLayout;
		std::optional<Key>		 indiceBuffer	= {};
		std::optional<Key>		 indirectBuffer = {};
		std::vector<BufferChunk> chunks;
	};

	/**
	 * @brief Indirect draw command descriptors.
	 */
	struct DrawIndirectCommand
	{
		uint32_t vertexCount;
		uint32_t instanceCount;
		uint32_t firstVertex;
		uint32_t baseInstance;
	};

	struct DrawIndirectRecord
	{
		DrawIndirectCommand command;
		uint32_t			idModel	 = 0;
		uint32_t			padding0 = 0;
		uint32_t			padding1 = 0;
		uint32_t			padding2 = 0;
	};

	struct DrawIndexedIndirectCommand
	{
		uint32_t indexCount;
		uint32_t instanceCount;
		uint32_t firstIndex;
		int32_t	 baseVertex;
		uint32_t baseInstance;
	};

	struct DrawIndexedIndirectRecord
	{
		DrawIndexedIndirectCommand command;
		uint32_t				   idModel	= 0;
		uint32_t				   padding0 = 0;
		uint32_t				   padding1 = 0;
	};

	static_assert( sizeof( DrawIndirectRecord ) == 32 );
	static_assert( sizeof( DrawIndexedIndirectRecord ) == 32 );

	inline constexpr uint32_t DRAW_INDIRECT_COUNT_OFFSET		= 0u;
	inline constexpr uint32_t DRAW_INDIRECT_COMMANDS_OFFSET		= 16u;
	inline constexpr uint32_t DRAW_INDIRECT_RECORD_SIZE			= sizeof( DrawIndirectRecord );
	inline constexpr uint32_t DRAW_INDEXED_INDIRECT_RECORD_SIZE = sizeof( DrawIndexedIndirectRecord );

	/**
	 * @brief Draw call descriptor.
	 */
	struct DrawCall
	{
		Key			geometry;
		E_PRIMITIVE primitive;

		struct Range
		{
			uint32_t				   first = 0;
			uint32_t				   count = 0;
			std::optional<BufferChunk> chunk = std::nullopt;
		};

		struct Indirect
		{
			std::optional<BufferChunk> chunk = std::nullopt;
		};

		// Could be a direct range, or an indirect draw buffer whose draw count lives on the GPU.
		using RangesVariant	 = std::variant<Range, Indirect>;
		RangesVariant ranges = Range { 0, 0 };
	};

	/**
	 * @brief Compute dispatch descriptors.
	 */
	struct Dispatch
	{
		uint32_t		 groupX	  = 1;
		uint32_t		 groupY	  = 1;
		uint32_t		 groupZ	  = 1;
		E_MEMORY_BARRIER barriers = E_MEMORY_BARRIER::NONE;
	};

	struct DispatchIndirect
	{
		Key				 indirectBuffer;
		uint32_t		 offset	  = 0;
		E_MEMORY_BARRIER barriers = E_MEMORY_BARRIER::NONE;
	};

	/**
	 * @brief Program descriptor.
	 */
	struct Program
	{
		Key								name;
		DirOrFiles						shaders;
		std::vector<UniformValue>		uniforms;
		std::optional<DrawCall>			drawCall;
		std::optional<Dispatch>			dispatch;
		std::optional<DispatchIndirect> dispatchIndirect;
	};

	/**
	 * @brief Pass descriptor.
	 */
	struct Pass
	{
		Key							 name;
		E_PASS_TYPE					 type	   = E_PASS_TYPE::GRAPHICS;
		E_PASS_EXECUTION			 execution = E_PASS_EXECUTION::EVERY_FRAME;
		std::vector<ResourceBinding> inputs;
		std::vector<ResourceBinding> outputs;
		std::vector<Program>		 programs;
		std::vector<E_SETTING>		 settings;
	};

	/**
	 * @brief All resources.
	 */
	template<typename T>
	using ResourceMap = std::unordered_map<Key, T>;

	struct Resources
	{
		ResourceMap<Texture>	  textures;
		ResourceMap<Sampler>	  samplers;
		ResourceMap<VertexLayout> vertexStreams;
		ResourceMap<Buffer>		  buffers;
		ResourceMap<Geometry>	  geometries;
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

	/**
	 * @brief Information about the availability of interop with other backends.
	 */
	struct InteropAvailability
	{
		/**
		 * @brief Shipped with this build.
		 */
		bool compiled = false;
		/**
		 * @brief Initlialized at runtime.
		 */
		bool runtime = false;

		/**
		 * @brief Availability.
		 */
		bool inline available() const noexcept { return compiled && runtime; }
	};

	/**
	 * @brief Buffer mapping for interop with external compute backends.
	 */
	struct InteropBufferMapping
	{
		/**
		 * @brief Key of the mapped buffer.
		 */
		Key key;
		/**
		 * @brief Generic device pointer.
		 */
		void * devicePtr = nullptr;

		/**
		 * @brief Size of the mapped buffer.
		 */
		size_t size = 0;
	};

} // namespace VTX::Renderer::Desc

#endif
