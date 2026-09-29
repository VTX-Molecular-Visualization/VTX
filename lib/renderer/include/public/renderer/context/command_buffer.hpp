#ifndef __VTX_RENDERER_CONTEXT_COMMAND_BUFFER__
#define __VTX_RENDERER_CONTEXT_COMMAND_BUFFER__

#include "renderer/descriptors.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <util/constants.hpp>
#include <util/enum.hpp>
#include <utility>
#include <vector>

namespace VTX::Renderer::Context
{

	/**
	 * @brief All command types.
	 */
	enum struct E_COMMAND : uint8_t
	{

		BEGIN_PASS,
		BIND_RESOURCES,
		DRAW,
		DRAW_INDEXED,
		DRAW_INDIRECT,
		DRAW_INDIRECT_READABLE,
		DRAW_INDEXED_INDIRECT,
		DRAW_INDEXED_INDIRECT_READABLE,
		DISPATCH,
		DISPATCH_INDIRECT,
		EXTERNAL,
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
		uint32_t flags	= 0;
		uint32_t width	= 0;
		uint32_t height = 0;
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
		uint32_t commandStride;
	};

	struct PayloadDrawIndirectReadable : PayloadDrawIndirect
	{
		uint32_t shaderStorageBinding;
	};

	struct PayloadDrawIndexedIndirect : BasePayloadDraw
	{
		uint32_t indirectBuffer;
		uint32_t indiceBuffer;
		uint32_t countOffset;
		uint32_t commandOffset;
		uint32_t commandStride;
	};

	struct PayloadDrawIndexedIndirectReadable : PayloadDrawIndexedIndirect
	{
		uint32_t shaderStorageBinding;
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
	struct CommandPayload<E_COMMAND::DRAW_INDIRECT_READABLE>
	{
		using type = PayloadDrawIndirectReadable;
	};

	template<>
	struct CommandPayload<E_COMMAND::DRAW_INDEXED_INDIRECT>
	{
		using type = PayloadDrawIndexedIndirect;
	};

	template<>
	struct CommandPayload<E_COMMAND::DRAW_INDEXED_INDIRECT_READABLE>
	{
		using type = PayloadDrawIndexedIndirectReadable;
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
	using PayloadT = CommandPayload<C>::type;

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
	using PayloadOffset = uint32_t;
	using CommandID		= uint32_t;
	using CommandIDList = std::vector<CommandID>;
	using PassID		= uint32_t;
	using PassIDList	= std::vector<PassID>;

	/**
	 * @brief Buffer that can grow dynamically and store contiguous payloads.
	 */
	class PayloadBuffer
	{
	  public:
		static constexpr std::size_t ALIGNMENT = 16;
		static constexpr std::size_t MAX_SIZE  = TypeMax<PayloadOffset>;

		PayloadBuffer() = default;

		std::size_t size() const { return _size; }

		bool empty() const { return _size == 0; }

		std::byte * data() { return _data.get(); }

		const std::byte * data() const { return _data.get(); }

		std::byte operator[]( const std::size_t p_index ) const
		{
			assert( p_index < _size );
			return _data[ p_index ];
		}

		void clear() { _size = 0; }

	  private:
		friend struct CommandBuffer;

		struct Deleter
		{
			void operator()( std::byte * const p_data ) const noexcept
			{ ::operator delete( p_data, std::align_val_t( ALIGNMENT ) ); }
		};

		using Storage = std::unique_ptr<std::byte[], Deleter>;

		/**
		 * @brief Double size or given size.
		 */
		void grow( const std::size_t p_size )
		{
			if ( p_size > MAX_SIZE )
			{
				throw std::length_error( "Command payload buffer is too large." );
			}
			assert( p_size >= _size );
			if ( p_size > _capacity )
			{
				const std::size_t doubled  = _capacity > MAX_SIZE - _capacity ? MAX_SIZE : _capacity * 2;
				const std::size_t capacity = p_size > doubled ? p_size : doubled;
				auto			  data = Storage( ::new ( ::operator new( capacity, std::align_val_t( ALIGNMENT ) ) )
													  std::byte[ capacity ] );
				if ( _size != 0 )
				{
					std::memcpy( data.get(), _data.get(), _size );
				}
				_data	  = std::move( data );
				_capacity = capacity;
			}
			_size = p_size;
		}

		/**
		 * @brief Payload data.
		 */
		Storage _data;

		/**
		 * @brief Current size and capacity of the buffer.
		 */
		std::size_t _size	  = 0;
		std::size_t _capacity = 0;
	};

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
		void clear()
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
		bool empty() const { return commands.empty(); }

		/**
		 * @brief Get current command ID.
		 */
		CommandID currentCommandID() const { return static_cast<CommandID>( commands.size() ); }

		/**
		 * @brief Begin a pass command range.
		 */
		PassID beginPass( const Desc::Pass & p_pass )
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
		void endPass( const PassID p_passID )
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
		PassID passID( const Desc::Key & p_pass ) const
		{
			const auto it = passIDByName.find( p_pass );
			assert( it != passIDByName.end() );
			return it->second;
		}

		bool containsPass( const Desc::Key & p_pass ) const { return passIDByName.contains( p_pass ); }

		const PassCommandRange & passRange( const PassID p_passID ) const
		{
			assert( p_passID < passes.size() );
			return passes[ p_passID ];
		}

		const PassCommandRange & passRange( const Desc::Key & p_pass ) const { return passRange( passID( p_pass ) ); }

		/**
		 * @brief Mark an ON_DIRTY pass for execution.
		 */
		bool markPassDirty( const Desc::Key & p_pass )
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
		void clearDirtyPasses() { onDirtyPassIDs.clear(); }

		/**
		 * @brief Get a payload from its offset.
		 */
		template<typename T>
		T & getPayload( const PayloadOffset p_offset )
		{
			static_assert( std::is_trivially_copyable_v<T> and std::is_trivially_copy_constructible_v<T> );
			static_assert( alignof( T ) <= PayloadBuffer::ALIGNMENT );
			assert( p_offset != NO_PAYLOAD );
			assert( p_offset <= payload.size() );
			assert( sizeof( T ) <= payload.size() - p_offset );
			assert( ( p_offset % alignof( T ) ) == 0 );

			return *std::launder( reinterpret_cast<T *>( payload.data() + p_offset ) );
		}

		template<typename T>
		const T & getPayload( const PayloadOffset p_offset ) const
		{
			static_assert( std::is_trivially_copyable_v<T> and std::is_trivially_copy_constructible_v<T> );
			static_assert( alignof( T ) <= PayloadBuffer::ALIGNMENT );
			assert( p_offset != NO_PAYLOAD );
			assert( p_offset <= payload.size() );
			assert( sizeof( T ) <= payload.size() - p_offset );
			assert( ( p_offset % alignof( T ) ) == 0 );

			return *std::launder( reinterpret_cast<const T *>( payload.data() + p_offset ) );
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
			static_assert( std::is_trivially_copyable_v<T> and std::is_trivially_copy_constructible_v<T> );
			static_assert( std::is_same_v<T, std::remove_cv_t<T>> );
			static_assert( alignof( T ) <= PayloadBuffer::ALIGNMENT );

			const std::size_t size	  = payload.size();
			const std::size_t padding = ( alignof( T ) - size % alignof( T ) ) % alignof( T );
			if ( padding > PayloadBuffer::MAX_SIZE - size || sizeof( T ) > PayloadBuffer::MAX_SIZE - size - padding )
			{
				throw std::length_error( "Command payload offset is out of range." );
			}
			const std::size_t offset = size + padding;
			const T			  data	 = p_data;
			payload.grow( offset + sizeof( T ) );

			// Add padding.
			if ( padding != 0 )
			{
				std::memset( payload.data() + size, 0, padding );
			}

			std::construct_at( reinterpret_cast<T *>( payload.data() + offset ), data );

			return static_cast<PayloadOffset>( offset );
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
