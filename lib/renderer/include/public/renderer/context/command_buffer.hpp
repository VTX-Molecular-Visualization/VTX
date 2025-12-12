#ifndef __VTX_RENDERER_CONTEXT_COMMAND_BUFFER__
#define __VTX_RENDERER_CONTEXT_COMMAND_BUFFER__

#include <cstdint>
#include <vector>

namespace VTX::Renderer::Context
{

	/**
	 * @brief All command types.
	 */
	enum struct E_COMMAND : std::uint8_t
	{
		BeginPass,
		EndPass,

		SetViewport,
		SetScissor,

		BindFramebuffer,
		BindPipeline,
		BindVertexBuffer,
		BindIndexBuffer,
		BindUniformBuffer,
		BindTexture,

		Draw,
		DrawIndexed,
		DispatchCompute,
	};

	/**
	 * @brief Payloads for each command type.
	 */
	struct CmdBeginPass
	{
		std::uint32_t passId;
	};

	struct CmdEndPass
	{
		std::uint32_t passId;
	};

	struct CmdSetViewport
	{
		std::int32_t x;
		std::int32_t y;
		std::int32_t width;
		std::int32_t height;
	};

	struct CmdSetScissor
	{
		std::int32_t x;
		std::int32_t y;
		std::int32_t width;
		std::int32_t height;
	};

	struct CmdBindFramebuffer
	{
		std::uint32_t framebufferId;
	};

	struct CmdBindPipeline
	{
		std::uint32_t pipelineId;
	};

	struct CmdBindVertexBuffer
	{
		std::uint32_t bufferId;
		std::uint32_t slot;
		std::uint32_t stride;
	};

	struct CmdBindIndexBuffer
	{
		std::uint32_t bufferId;
	};

	struct CmdBindUniformBuffer
	{
		std::uint32_t bufferId;
		std::uint32_t binding;
	};

	struct CmdBindTexture
	{
		std::uint32_t textureId;
		std::uint32_t unit;
	};

	struct CmdDraw
	{
		std::uint32_t vertexCount;
		std::uint32_t firstVertex;
	};

	struct CmdDrawIndexed
	{
		std::uint32_t indexCount;
		std::uint32_t firstIndex;
		std::int32_t  vertexOffset;
	};

	struct CmdDispatchCompute
	{
		std::uint32_t groupsX;
		std::uint32_t groupsY;
		std::uint32_t groupsZ;
	};

	/**
	 * @brief Command structure.
	 */
	struct Command
	{
		E_COMMAND	  type;
		std::uint32_t payloadOffset;
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
		std::vector<std::uint8_t> payload;

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
		T & getPayload( const std::uint32_t p_offset )
		{
			return *reinterpret_cast<T *>( payload.data() + p_offset );
		}

		template<typename T>
		const T & getPayload( const std::uint32_t p_offset ) const
		{
			return *reinterpret_cast<const T *>( payload.data() + p_offset );
		}

		/**
		 * @brief Push a command with its payload.
		 */
		template<typename T>
		void push( const E_COMMAND p_type, const T & p_data )
		{
			const std::uint32_t offset = pushPayload( p_data );
			commands.push_back( Command { p_type, offset } );
		}

		/**
		 * @brief Push a command with its payload.
		 * @return The offset of the payload in the payload buffer.
		 */
		template<typename T>
		std::uint32_t pushPayload( const T & p_data )
		{
			const std::uint32_t offset = static_cast<std::uint32_t>( payload.size() );
			const auto *		src	   = reinterpret_cast<const std::uint8_t *>( &p_data );

			payload.insert( payload.end(), src, src + sizeof( T ) );

			return offset;
		}
	};
} // namespace VTX::Renderer::Context

#endif
