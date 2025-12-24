#ifndef __VTX_RENDERER_CONTEXT_COMMAND_BUFFER__
#define __VTX_RENDERER_CONTEXT_COMMAND_BUFFER__

#include "renderer/descriptors.hpp"
#include <cstdint>
#include <util/constants.hpp>
#include <util/math.hpp>
#include <vector>

namespace VTX::Renderer::Context
{

	/**
	 * @brief All command types.
	 */
	enum struct E_COMMAND : std::uint8_t
	{
		CLEAR,

		BEGIN_PASS,
		END_PASS,

		SET_VIEWPORT,

		BIND_PIPELINE,
		BIND_RESOURCE_TABLE,
		PUSH_CONSTANTS,

		BIND_GEOMETRY,
		DRAW,
		DRAW_INDEXED,
		DRAW_INDIRECT,
		DRAW_INDEXED_INDIRECT,
		DISPATCH,

		BARRIER
	};

	/**
	 * @brief Pipeline types.
	 */
	enum struct E_PIPELINE_TYPE : std::uint8_t
	{
		GRAPHICS,
		COMPUTE
	};

	/**
	 * @brief Index types.
	 */
	enum struct E_INDEX_TYPE : std::uint8_t
	{
		U16,
		U32
	};

	/**
	 * @brief Clear mask bits.
	 */
	enum : std::uint32_t
	{
		CLEAR_COLOR	  = 1u << 0,
		CLEAR_DEPTH	  = 1u << 1,
		CLEAR_STENCIL = 1u << 2
	};

	/**
	 * @brief Shader stages.
	 */
	enum : std::uint32_t
	{
		STAGE_VERTEX   = 1u << 0,
		STAGE_FRAGMENT = 1u << 1,
		STAGE_COMPUTE  = 1u << 2,
		STAGE_ALL	   = 0xFFFF'FFFFu
	};

	/**
	 * @brief No payload.
	 */
	constexpr uint32_t NO_PAYLOAD = TypeMax<uint32_t>;

	/**
	 * @brief Payloads for each command type.
	 */
	struct PayloadClear
	{
	};

	struct PayloadBeginPass
	{
		Handle	 renderTarget;
		uint32_t clearFlags;
		float	 clearColor[ 4 ];
		float	 clearDepth;
		int32_t	 clearStencil;
		uint8_t	 enableDepthTest;
		uint8_t	 _pad[ 3 ] {};
	};

	struct PayloadEndPass
	{
		uint8_t disableDepthTest;
		uint8_t _pad[ 3 ] {};
	};

	struct PayloadViewport
	{
		int32_t x, y;
		int32_t w, h;
	};

	struct PayloadBindPipeline
	{
		E_PIPELINE_TYPE type;
		uint8_t			_pad[ 3 ] {};
		Handle			pipeline;
	};

	struct PayloadBindResourceTable
	{
		uint32_t set;
		Handle	 table;
	};

	// ?
	struct PayloadPushConstants
	{
		uint32_t stageMask;
		uint32_t size;
		uint32_t dataOffset;
	};

	struct PayloadBindGeometry
	{
		Handle geometry;
	};

	struct PayloadDraw
	{
		uint32_t vertexCount;
		uint32_t instanceCount;
		uint32_t firstVertex;
		uint32_t firstInstance;
	};

	struct PayloadDrawIndexed
	{
		uint32_t	 indexCount;
		uint32_t	 instanceCount;
		uint32_t	 firstIndex;
		int32_t		 vertexOffset;
		uint32_t	 firstInstance;
		E_INDEX_TYPE indexType;
		uint8_t		 _pad[ 3 ] {};
		uint32_t	 indexBufferOffsetBytes;
	};

	struct PayloadDrawIndirect
	{
		Handle	 indirectBuffer;
		uint32_t offsetBytes;
		uint32_t drawCount;
		uint32_t strideBytes;
	};

	struct PayloadDrawIndexedIndirect
	{
		Handle	 indirectBuffer;
		uint32_t offsetBytes;
		uint32_t drawCount;
		uint32_t strideBytes;
	};

	struct PayloadDispatch
	{
		uint32_t x, y, z;
	};

	struct PayloadBarrier
	{
		uint32_t srcStageMask;
		uint32_t dstStageMask;
		uint32_t accessMask;
	};

	/**
	 * @brief Command payload specializations.
	 */
	template<E_COMMAND>
	struct CommandPayload;
	template<>
	struct CommandPayload<E_COMMAND::CLEAR>
	{
		using type = PayloadClear;
	};
	template<>
	struct CommandPayload<E_COMMAND::BEGIN_PASS>
	{
		using type = PayloadBeginPass;
	};
	template<>
	struct CommandPayload<E_COMMAND::END_PASS>
	{
		using type = PayloadEndPass;
	};
	template<>
	struct CommandPayload<E_COMMAND::SET_VIEWPORT>
	{
		using type = PayloadViewport;
	};
	template<>
	struct CommandPayload<E_COMMAND::BIND_PIPELINE>
	{
		using type = PayloadBindPipeline;
	};
	template<>
	struct CommandPayload<E_COMMAND::BIND_RESOURCE_TABLE>
	{
		using type = PayloadBindResourceTable;
	};
	template<>
	struct CommandPayload<E_COMMAND::PUSH_CONSTANTS>
	{
		using type = PayloadPushConstants;
	};
	template<>
	struct CommandPayload<E_COMMAND::BIND_GEOMETRY>
	{
		using type = PayloadBindGeometry;
	};
	template<>
	struct CommandPayload<E_COMMAND::DRAW>
	{
		using type = PayloadDraw;
	};
	template<>
	struct CommandPayload<E_COMMAND::DRAW_INDEXED>
	{
		using type = PayloadDrawIndexed;
	};
	template<>
	struct CommandPayload<E_COMMAND::DRAW_INDIRECT>
	{
		using type = PayloadDrawIndirect;
	};
	template<>
	struct CommandPayload<E_COMMAND::DRAW_INDEXED_INDIRECT>
	{
		using type = PayloadDrawIndexedIndirect;
	};
	template<>
	struct CommandPayload<E_COMMAND::DISPATCH>
	{
		using type = PayloadDispatch;
	};
	template<>
	struct CommandPayload<E_COMMAND::BARRIER>
	{
		using type = PayloadBarrier;
	};
	template<E_COMMAND C>
	using PayloadT = typename CommandPayload<C>::type;

	/**
	 * @brief Command structure.
	 */
	struct Command
	{
		E_COMMAND type;
		uint32_t  payloadOffset;
	};

	/**
	 * @brief Command buffer that store pipelined commands and their payloads.
	 */
	struct CommandBuffer
	{
		/**
		 * @brief Commands list.
		 */
		std::vector<Command> commands;

		/**
		 * @brief Payload buffer.
		 */
		std::vector<std::byte> payload;

		/**
		 * @brief Clear all.
		 */
		inline void clear()
		{
			commands.clear();
			payload.clear();
		}

		/**
		 * @brief Check if empty.
		 */
		inline bool empty() const { return commands.empty(); }

		/**
		 * @brief Get a payload from its offset.
		 */
		template<typename T>
		T & getPayload( const uint32_t p_offset )
		{
			assert( p_offset != NO_PAYLOAD );
			assert( p_offset + sizeof( T ) <= payload.size() );
			assert( ( p_offset % alignof( T ) ) == 0 );

			return *reinterpret_cast<T *>( payload.data() + p_offset );
		}

		template<typename T>
		const T & getPayload( const uint32_t p_offset ) const
		{
			assert( p_offset != NO_PAYLOAD );
			assert( p_offset + sizeof( T ) <= payload.size() );
			assert( ( p_offset % alignof( T ) ) == 0 );

			return *reinterpret_cast<const T *>( payload.data() + p_offset );
		}

		/**
		 * @brief Push a command (no payload).
		 */
		template<E_COMMAND C>
		void push()
		{
			static_assert( std::is_empty_v<PayloadT<C>>, "This command requires a payload." );

			commands.push_back( Command { C, NO_PAYLOAD } );
		}

		/**
		 * @brief Push a command with its payload.
		 */
		template<E_COMMAND C>
		void push( const PayloadT<C> & p_data )
		{
			static_assert( not std::is_empty_v<PayloadT<C>>, "This command has no payload." );

			const uint32_t offset = pushPayload( p_data );
			commands.push_back( Command { C, offset } );
		}

		/**
		 * @brief Push data in the payload buffer and return its offset.
		 */
		template<typename T>
		uint32_t pushPayload( const T & p_data )
		{
			static_assert( std::is_trivially_copyable_v<T> );

			constexpr uint32_t A	   = static_cast<uint32_t>( alignof( T ) );
			uint32_t		   offset  = static_cast<uint32_t>( payload.size() );
			uint32_t		   aligned = Util::Math::alignUp( offset, A );

			// Add padding.
			if ( aligned != offset )
			{
				payload.insert( payload.end(), aligned - offset, std::byte { 0 } );
			}

			offset = aligned;

			const auto * src = reinterpret_cast<const std::byte *>( &p_data );
			payload.insert( payload.end(), src, src + sizeof( T ) );

			return offset;
		}
	};
} // namespace VTX::Renderer::Context

#endif
