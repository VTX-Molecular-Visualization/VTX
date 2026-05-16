#ifndef __VTX_RENDERER_CONTEXT_COMMAND_BUFFER__
#define __VTX_RENDERER_CONTEXT_COMMAND_BUFFER__

#include "renderer/descriptors.hpp"
#include <cassert>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <util/constants.hpp>
#include <util/enum.hpp>
#include <util/math.hpp>
#include <vector>

namespace VTX::Renderer::Context
{

	/**
	 * @brief All command types.
	 */
	enum struct E_COMMAND : uint32_t
	{

		BEGIN_PASS,
		BIND_RESOURCES,
		DRAW,
		DRAW_INDEXED,
		DRAW_INDIRECT,
		DRAW_INDEXED_INDIRECT,
		DISPATCH,
		DISPATCH_INDIRECT,
		EXTERNAL,
		BIND_OUTPUT,
		END_PASS,
		PRESENT
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
		uint32_t framebuffer;
		uint32_t flags = 0;
	};

	struct PayloadBindResources
	{
		uint32_t resourceTable;
	};

	struct PayloadBindPipeline
	{
		uint32_t pipeline;
	};

	struct BasePayloadDraw
	{
		uint32_t program;
		uint32_t pipeline;
		uint32_t primitive;
	};

	struct PayloadDraw : BasePayloadDraw
	{
		uint64_t first;
		uint64_t count;
	};

	struct PayloadDrawIndexed : BasePayloadDraw
	{
		uint32_t indexBuffer;
		uint64_t first;
		uint64_t count;
	};

	struct PayloadDrawIndirect : BasePayloadDraw
	{
		uint32_t indirectBuffer;
		uint32_t countOffset;
		uint32_t commandOffset;
	};

	struct PayloadDrawIndexedIndirect : BasePayloadDraw
	{
		uint32_t indirectBuffer;
		uint32_t indiceBuffer;
		uint32_t countOffset;
		uint32_t commandOffset;
	};

	struct BasePayloadDispatch
	{
		uint32_t program;
		uint32_t barriers;
	};

	struct PayloadDispatch : BasePayloadDispatch
	{
		uint32_t groupX;
		uint32_t groupY;
		uint32_t groupZ;
	};

	struct PayloadDispatchIndirect : BasePayloadDispatch
	{
		uint32_t indirectBuffer;
		uint32_t offset;
	};

	struct PayloadExternal
	{
		uintptr_t function = 0;
		uintptr_t context  = 0;
	};

	struct PayloadBindOutput
	{
		uint32_t framebuffer;
	};

	struct PayloadEndPass
	{
		uint32_t framebuffer;
		uint32_t flags = 0;
	};

	struct PayloadPresent
	{
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
	struct CommandPayload<E_COMMAND::DISPATCH_INDIRECT>
	{
		using type = PayloadDispatchIndirect;
	};

	template<>
	struct CommandPayload<E_COMMAND::EXTERNAL>
	{
		using type = PayloadExternal;
	};

	template<>
	struct CommandPayload<E_COMMAND::BIND_OUTPUT>
	{
		using type = PayloadBindOutput;
	};

	template<>
	struct CommandPayload<E_COMMAND::END_PASS>
	{
		using type = PayloadEndPass;
	};

	template<>
	struct CommandPayload<E_COMMAND::PRESENT>
	{
		using type = PayloadPresent;
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
	 * @brief Aliases.
	 */
	using CommandList	= std::vector<Command>;
	using PayloadBuffer = std::vector<std::byte>;
	using PayloadOffset = uint32_t;
	using CommandID		= uint32_t;
	using CommandIDList = std::vector<CommandID>;
	using PassID		= uint32_t;
	using PassIDList	= std::vector<PassID>;

	struct CommandRange
	{
		CommandID first = 0;
		uint32_t  count = 0;
	};

	struct PassCommandRange
	{
		Desc::Key			   name;
		Desc::E_PASS_EXECUTION execution = Desc::E_PASS_EXECUTION::EVERY_FRAME;
		CommandRange		   commands;
	};

	/**
	 * @brief Command buffer that store pipelined commands and their payloads.
	 */
	struct CommandBuffer
	{
		/**
		 * @brief Commands list.
		 */
		CommandList commands;

		/**
		 * @brief Payload buffer.
		 */
		PayloadBuffer payload;

		/**
		 * @brief Pass command ranges and IDs for each execution type.
		 */
		std::vector<PassCommandRange>		  passes;
		std::unordered_map<Desc::Key, PassID> passIDByName;
		PassIDList							  everyFramePassIDs;
		PassIDList							  onDirtyPassIDs;

		/**
		 * @brief Clear all.
		 */
		inline void clear()
		{
			commands.clear();
			payload.clear();
			passes.clear();
			passIDByName.clear();
			everyFramePassIDs.clear();
			onDirtyPassIDs.clear();
		}

		/**
		 * @brief Check if empty.
		 */
		inline bool empty() const { return commands.empty(); }

		/**
		 * @brief Get current command ID.
		 */
		inline CommandID currentCommandID() const { return static_cast<CommandID>( commands.size() ); }

		/**
		 * @brief Begin a pass command range.
		 */
		inline PassID beginPass( const Desc::Pass & p_pass )
		{
			const PassID id = static_cast<PassID>( passes.size() );

			passes.emplace_back(
				PassCommandRange {
					.name	   = p_pass.name,
					.execution = p_pass.execution,
					.commands  = CommandRange { .first = currentCommandID(), .count = 0 },
				}
			);

			const auto [ it, inserted ] = passIDByName.emplace( p_pass.name, id );
			assert( inserted );

			return id;
		}

		/**
		 * @brief End a pass command range.
		 */
		inline void endPass( const PassID p_passID )
		{
			assert( p_passID < passes.size() );

			PassCommandRange & pass = passes[ p_passID ];
			assert( pass.commands.count == 0 );

			pass.commands.count = currentCommandID() - pass.commands.first;

			switch ( pass.execution )
			{
			case Desc::E_PASS_EXECUTION::EVERY_FRAME: everyFramePassIDs.emplace_back( p_passID ); break;
			case Desc::E_PASS_EXECUTION::ON_DIRTY: break;
			default: assert( false ); break;
			}
		}

		/**
		 * @brief Get pass command range info.
		 */
		inline PassID passID( const Desc::Key & p_pass ) const
		{
			const auto it = passIDByName.find( p_pass );
			assert( it != passIDByName.end() );
			return it->second;
		}

		inline bool containsPass( const Desc::Key & p_pass ) const { return passIDByName.contains( p_pass ); }

		inline const PassCommandRange & passRange( const PassID p_passID ) const
		{
			assert( p_passID < passes.size() );
			return passes[ p_passID ];
		}

		inline const PassCommandRange & passRange( const Desc::Key & p_pass ) const
		{
			return passRange( passID( p_pass ) );
		}

		/**
		 * @brief Mark an ON_DIRTY pass for execution.
		 */
		inline bool markPassDirty( const Desc::Key & p_pass )
		{
			const PassID id = passID( p_pass );

			assert( passes[ id ].execution == Desc::E_PASS_EXECUTION::ON_DIRTY );

			for ( const PassID dirtyPassID : onDirtyPassIDs )
			{
				if ( dirtyPassID == id )
				{
					return true;
				}
			}

			onDirtyPassIDs.emplace_back( id );
			return true;
		}

		/**
		 * @brief Clear current dirty pass list.
		 */
		inline void clearDirtyPasses() { onDirtyPassIDs.clear(); }

		/**
		 * @brief Get a payload from its offset.
		 */
		template<typename T>
		T & getPayload( const PayloadOffset p_offset )
		{
			assert( p_offset != NO_PAYLOAD );
			assert( p_offset + sizeof( T ) <= payload.size() );
			assert( ( p_offset % alignof( T ) ) == 0 );

			return *reinterpret_cast<T *>( payload.data() + p_offset );
		}

		template<typename T>
		const T & getPayload( const PayloadOffset p_offset ) const
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
		CommandID push()
		{
			static_assert( std::is_empty_v<PayloadT<C>>, "This command requires a payload." );

			const CommandID id = currentCommandID();
			commands.push_back( Command { C, NO_PAYLOAD } );

			return id;
		}

		/**
		 * @brief Push a command with its payload.
		 */
		template<E_COMMAND C>
		CommandID push( const PayloadT<C> & p_data )
		{
			static_assert( not std::is_empty_v<PayloadT<C>>, "This command has no payload." );

			const CommandID		id	   = currentCommandID();
			const PayloadOffset offset = pushPayload( p_data );
			commands.push_back( Command { C, offset } );

			return id;
		}

		/**
		 * @brief Push data in the payload buffer and return its offset.
		 */
		template<typename T>
		PayloadOffset pushPayload( const T & p_data )
		{
			static_assert( std::is_trivially_copyable_v<T> );

			constexpr PayloadOffset A		= static_cast<PayloadOffset>( alignof( T ) );
			PayloadOffset			offset	= static_cast<PayloadOffset>( payload.size() );
			PayloadOffset			aligned = Util::Math::alignUp( offset, A );

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
