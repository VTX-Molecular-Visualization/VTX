#ifndef __VTX_RENDERER_CONTEXT_COMMAND_BUFFER__
#define __VTX_RENDERER_CONTEXT_COMMAND_BUFFER__

#include "renderer/descriptors.hpp"
#include <cstdint>
#include <iostream>
#include <util/constants.hpp>
#include <util/enum.hpp>
#include <util/math.hpp>
#include <vector>

namespace VTX::Renderer::Context
{

	/**
	 * @brief All command types.
	 */
	enum struct E_COMMAND : std::uint8_t
	{

		BEGIN_PASS,
		BIND_RESOURCES,
		END_PASS,

	};

	/**
	 * @brief No payload.
	 */
	constexpr uint32_t NO_PAYLOAD = TypeMax<uint32_t>;

	/**
	 * @brief Payloads for each command type.
	 */
	struct PayloadBeginPass
	{
		Handle	 framebuffer;
		uint32_t flags = 0;
	};

	struct PayloadEndPass
	{
		uint32_t flags = 0;
	};

	struct PayloadBindResources
	{
		Handle resourceTable;
	};

	/**
	 * @brief Command payload specializations.
	 */
	template<E_COMMAND>
	struct CommandPayload;
	template<>
	struct CommandPayload<E_COMMAND::BEGIN_PASS>
	{
		using type = PayloadBeginPass;
	};
	template<>
	struct CommandPayload<E_COMMAND::BIND_RESOURCES>
	{
		using type = PayloadBindResources;
	};
	template<>
	struct CommandPayload<E_COMMAND::END_PASS>
	{
		using type = PayloadEndPass;
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

		/**
		 * @brief Debug output operator.
		 */
		friend std::ostream & operator<<( std::ostream &, const CommandBuffer & p_cb )
		{
			std::ostream & os = std::cout;
			os << "CommandBuffer: " << std::endl;
			for ( const Command & cmd : p_cb.commands )
			{
				os << "  Command Type: " << Util::Enum::enumName( cmd.type )
				   << ", Payload Offset: " << cmd.payloadOffset << std::endl;
			}

			return os;
		}
	};
} // namespace VTX::Renderer::Context

#endif
