#include "renderer/context/context_wrapper.hpp"
#include "renderer/context/backend/opengl.hpp"
#include "renderer/context/executor/null.hpp"
#include "renderer/context/executor/opengl.hpp"
#include <array>
#include <type_traits>
#include <util/enum.hpp>
#include <util/logger.hpp>
#include <utility>
#include <variant>

namespace VTX::Renderer::Context
{
	/**
	 * @brief Available backends and executors.
	 */
	using BackendVariant  = std::variant<std::monostate, Backend::OpenGL>;
	using ExecutorVariant = std::variant<std::monostate, Executor::Null, Executor::OpenGL>;

	/**
	 * @brief Implementation struct.
	 */
	struct ContextWrapper::Impl
	{ /**
	   * @brief Command buffer to fill and execute.
	   */
		CommandBuffer commands;

		/**
		 * @brief Current backend.
		 */
		BackendVariant backend;

		/**
		 * @brief Current executor.
		 */
		ExecutorVariant executor;
	};

	// Helpers.
	namespace
	{
		/**
		 * @brief Set the graphic backend.
		 */
		template<typename B, typename... Args>
		void _setBackend( BackendVariant & p_backend, ExecutorVariant & p_executor, Args &&... p_args )
		{
			p_backend.emplace<B>( std::forward<Args>( p_args )... );

			// Rebuild executor to match the new backend.
			if constexpr ( std::is_same_v<B, Backend::OpenGL> )
			{
				if ( auto * executor = std::get_if<Executor::OpenGL>( &p_executor ) )
				{
					p_executor.emplace<Executor::OpenGL>( std::get<Backend::OpenGL>( p_backend ) );
				}
			}
		}

		/**
		 * @brief Set the command buffer executor.
		 */
		template<typename E>
		void _setExecutor( const BackendVariant & p_backend, ExecutorVariant & p_executor )
		{
			if constexpr ( std::is_same_v<E, Executor::OpenGL> )
			{
				if ( auto * backend = std::get_if<Backend::OpenGL>( &p_backend ) )
				{
					p_executor.emplace<Executor::OpenGL>( *backend );
					return;
				}
			}

			p_executor.emplace<Executor::Null>();
		}

		template<E_COMMAND C, typename Executor>
		void _executePayload( const Executor & p_executor, const CommandBuffer & p_commands, const Command & p_command )
		{
			using Payload = PayloadT<C>;

			if constexpr ( std::is_empty_v<Payload> )
			{
				p_executor.execute( Payload {} );
			}
			else
			{
				p_executor.execute( p_commands.getPayload<Payload>( p_command.payloadOffset ) );
			}
		}

		template<typename Executor>
		void _executeCommand( const Executor & p_executor, const CommandBuffer & p_commands, const Command & p_command )
		{
			switch ( p_command.type )
			{
			case E_COMMAND::BEGIN_PASS:
				_executePayload<E_COMMAND::BEGIN_PASS>( p_executor, p_commands, p_command );
				break;
			case E_COMMAND::BIND_RESOURCES:
				_executePayload<E_COMMAND::BIND_RESOURCES>( p_executor, p_commands, p_command );
				break;
			case E_COMMAND::DRAW: _executePayload<E_COMMAND::DRAW>( p_executor, p_commands, p_command ); break;
			case E_COMMAND::DRAW_INDEXED:
				_executePayload<E_COMMAND::DRAW_INDEXED>( p_executor, p_commands, p_command );
				break;
			case E_COMMAND::DRAW_INDIRECT:
				_executePayload<E_COMMAND::DRAW_INDIRECT>( p_executor, p_commands, p_command );
				break;
			case E_COMMAND::DRAW_INDIRECT_READABLE:
				_executePayload<E_COMMAND::DRAW_INDIRECT_READABLE>( p_executor, p_commands, p_command );
				break;
			case E_COMMAND::DRAW_INDEXED_INDIRECT:
				_executePayload<E_COMMAND::DRAW_INDEXED_INDIRECT>( p_executor, p_commands, p_command );
				break;
			case E_COMMAND::DRAW_INDEXED_INDIRECT_READABLE:
				_executePayload<E_COMMAND::DRAW_INDEXED_INDIRECT_READABLE>( p_executor, p_commands, p_command );
				break;
			case E_COMMAND::DISPATCH: _executePayload<E_COMMAND::DISPATCH>( p_executor, p_commands, p_command ); break;
			case E_COMMAND::DISPATCH_INDIRECT:
				_executePayload<E_COMMAND::DISPATCH_INDIRECT>( p_executor, p_commands, p_command );
				break;
			case E_COMMAND::EXTERNAL: _executePayload<E_COMMAND::EXTERNAL>( p_executor, p_commands, p_command ); break;
			case E_COMMAND::BIND_OUTPUT:
				_executePayload<E_COMMAND::BIND_OUTPUT>( p_executor, p_commands, p_command );
				break;
			case E_COMMAND::END_PASS: _executePayload<E_COMMAND::END_PASS>( p_executor, p_commands, p_command ); break;
			case E_COMMAND::PRESENT: _executePayload<E_COMMAND::PRESENT>( p_executor, p_commands, p_command ); break;
			default: break;
			}
		}

		template<typename Executor>
		void _executeRange(
			const Executor &	  p_executor,
			const CommandBuffer & p_commands,
			const CommandRange &  p_range
		)
		{
			const CommandID end = p_range.first + p_range.count;
			assert( end <= p_commands.commands.size() );

			for ( CommandID commandID = p_range.first; commandID < end; commandID++ )
			{
				_executeCommand( p_executor, p_commands, p_commands.commands[ commandID ] );
			}
		}

		void _logInteropAvailability(
			const Desc::E_INTEROP_API		  p_api,
			const Desc::InteropAvailability & p_availability
		)
		{
			const std::string_view apiName = Util::Enum::enumName( p_api );

			if ( not p_availability.compiled )
			{
				VTX_TRACE( "{} not shipped", apiName );
			}
			else if ( p_availability.available() )
			{
				VTX_INFO( "{} available", apiName );
			}
			else
			{
				VTX_TRACE( "{} not available", apiName );
			}
		}
	} // namespace

	ContextWrapper::ContextWrapper() : _impl( std::make_unique<Impl>() ) {}

	ContextWrapper::~ContextWrapper() = default;

	void ContextWrapper::setOpenGL(
		const size_t					p_width,
		const size_t					p_height,
		const Desc::NativeContextInfo & p_contextInfo,
		const FilePath &				p_shaderDir
	)
	{
		_setBackend<Backend::OpenGL>( _impl->backend, _impl->executor, p_width, p_height, p_contextInfo, p_shaderDir );
		_setExecutor<Executor::OpenGL>( _impl->backend, _impl->executor );
		_logInteropAvailability( Desc::E_INTEROP_API::CUDA, interopAvailability( Desc::E_INTEROP_API::CUDA ) );
		_logInteropAvailability( Desc::E_INTEROP_API::OPTIX, interopAvailability( Desc::E_INTEROP_API::OPTIX ) );
	}

	void ContextWrapper::setNull()
	{
		_setBackend<std::monostate>( _impl->backend, _impl->executor );
		_setExecutor<Executor::Null>( _impl->backend, _impl->executor );
	}

	void ContextWrapper::execute() const noexcept
	{
		if ( not _impl->commands.onDirtyPassIDs.empty() )
		{
			execute(
				std::span<const PassID> {
					_impl->commands.onDirtyPassIDs.data(),
					_impl->commands.onDirtyPassIDs.size(),
				}
			);
			_impl->commands.clearDirtyPasses();
		}

		execute(
			std::span<const PassID> {
				_impl->commands.everyFramePassIDs.data(),
				_impl->commands.everyFramePassIDs.size(),
			}
		);
	}

	void ContextWrapper::execute( std::span<const PassID> p_passIDs ) const noexcept
	{
		std::visit(
			[ & ]( const auto & p_exec )
			{
				using T = std::remove_cvref_t<decltype( p_exec )>;
				if constexpr ( not std::is_same_v<T, std::monostate> )
				{
					for ( const PassID passID : p_passIDs )
					{
						_executeRange( p_exec, _impl->commands, _impl->commands.passRange( passID ).commands );
					}
				}
			},
			_impl->executor
		);
	}

	void ContextWrapper::build( const Desc::RenderQueue & p_renderQueue, const Desc::Resources & p_resources )
	{
		_impl->commands.clear();

		std::visit(
			[ & ]( auto & p_backend )
			{
				using T = std::remove_cvref_t<decltype( p_backend )>;
				if constexpr ( not std::is_same_v<T, std::monostate> )
				{
					p_backend.build( p_renderQueue, p_resources, _impl->commands );
				}
			},
			_impl->backend
		);
	}

	void ContextWrapper::setExternalPass(
		const Desc::Key & p_pass,
		const uintptr_t	  p_function,
		const uintptr_t	  p_context
	)
	{
		const PassCommandRange & pass	  = _impl->commands.passRange( p_pass );
		const CommandRange &	 commands = pass.commands;

		assert( commands.count == 1 );

		Command & command = _impl->commands.commands[ commands.first ];
		assert( command.type == E_COMMAND::EXTERNAL );

		PayloadExternal & payload = _impl->commands.getPayload<PayloadExternal>( command.payloadOffset );
		payload.function		  = p_function;
		payload.context			  = p_context;
	}

	bool ContextWrapper::containsPass( const Desc::Key & p_pass ) const
	{
		return _impl->commands.containsPass( p_pass );
	}

	void ContextWrapper::markPassDirty( const Desc::Key & p_pass ) { _impl->commands.markPassDirty( p_pass ); }

	void ContextWrapper::resize(
		const uint32_t										 p_width,
		const uint32_t										 p_height,
		const Desc::PassList &								 p_passes,
		const std::unordered_map<Desc::Key, Desc::Texture> & p_textures
	)
	{
		std::visit(
			[ & ]( auto & p_backend )
			{
				using T = std::remove_cvref_t<decltype( p_backend )>;
				if constexpr ( not std::is_same_v<T, std::monostate> )
				{
					p_backend.resize( p_width, p_height, p_passes, p_textures );
				}
			},
			_impl->backend
		);
	}

	void ContextWrapper::setBuffer( const Desc::BufferRef & p_ref, SpanBytes p_bytes, const size_t p_offset )
	{
		if ( p_bytes.size() == 0 )
		{
			return;
		}

		std::visit(
			[ & ]( auto & p_backend )
			{
				using T = std::remove_cvref_t<decltype( p_backend )>;
				if constexpr ( std::is_same_v<T, Backend::OpenGL> )
				{
					p_backend.setBufferData( p_ref, p_bytes, p_offset );
				}
			},
			_impl->backend
		);
	}

	bool ContextWrapper::ensureBufferChunk( const Desc::BufferRef & p_ref )
	{
		return std::visit(
			[ & ]( auto & p_backend )
			{
				using T = std::remove_cvref_t<decltype( p_backend )>;
				if constexpr ( std::is_same_v<T, Backend::OpenGL> )
				{
					return p_backend.ensureBufferChunk( p_ref );
				}
				else
				{
					return false;
				}
			},
			_impl->backend
		);
	}

	bool ContextWrapper::releaseBufferChunk( const Desc::BufferRef & p_ref )
	{
		return std::visit(
			[ & ]( auto & p_backend )
			{
				using T = std::remove_cvref_t<decltype( p_backend )>;
				if constexpr ( std::is_same_v<T, Backend::OpenGL> )
				{
					return p_backend.releaseBufferChunk( p_ref );
				}
				else
				{
					return false;
				}
			},
			_impl->backend
		);
	}

	Desc::InteropBufferMapping ContextWrapper::mapInteropBuffer(
		const Desc::E_INTEROP_API p_api,
		const Desc::BufferRef &	  p_ref
	)
	{
		std::array<Desc::BufferRef, 1> refs { p_ref };
		auto						   mappings = mapInteropBuffers( p_api, refs );
		assert( mappings.size() == 1 );
		return std::move( mappings.front() );
	}

	std::vector<Desc::InteropBufferMapping> ContextWrapper::mapInteropBuffers(
		const Desc::E_INTEROP_API		 p_api,
		std::span<const Desc::BufferRef> p_refs
	)
	{
		return std::visit(
			[ & ]( auto & p_backend ) -> std::vector<Desc::InteropBufferMapping>
			{
				using T = std::remove_cvref_t<decltype( p_backend )>;
				if constexpr ( std::is_same_v<T, Backend::OpenGL> )
				{
					return p_backend.mapInteropBuffers( p_api, p_refs );
				}
				else
				{
					return {};
				}
			},
			_impl->backend
		);
	}

	void ContextWrapper::unmapInteropBuffer(
		const Desc::E_INTEROP_API		   p_api,
		const Desc::InteropBufferMapping & p_mapping
	)
	{
		std::array<Desc::InteropBufferMapping, 1> mappings { p_mapping };
		unmapInteropBuffers( p_api, mappings );
	}

	void ContextWrapper::unmapInteropBuffers(
		const Desc::E_INTEROP_API					p_api,
		std::span<const Desc::InteropBufferMapping> p_mappings
	)
	{
		std::visit(
			[ & ]( auto & p_backend )
			{
				using T = std::remove_cvref_t<decltype( p_backend )>;
				if constexpr ( std::is_same_v<T, Backend::OpenGL> )
				{
					p_backend.unmapInteropBuffers( p_api, p_mappings );
				}
			},
			_impl->backend
		);
	}

	void ContextWrapper::unregisterInteropBuffer( const Desc::E_INTEROP_API p_api, const Desc::BufferRef & p_ref )
	{
		std::visit(
			[ & ]( auto & p_backend )
			{
				using T = std::remove_cvref_t<decltype( p_backend )>;
				if constexpr ( std::is_same_v<T, Backend::OpenGL> )
				{
					p_backend.unregisterInteropBuffer( p_api, p_ref );
				}
			},
			_impl->backend
		);
	}

	Desc::InteropAvailability ContextWrapper::interopAvailability( const Desc::E_INTEROP_API p_api ) const
	{
		return std::visit(
			[ & ]( const auto & p_backend ) -> Desc::InteropAvailability
			{
				using T = std::remove_cvref_t<decltype( p_backend )>;
				if constexpr ( std::is_same_v<T, Backend::OpenGL> )
				{
					return p_backend.interopAvailability( p_api );
				}
				else
				{
					return {};
				}
			},
			_impl->backend
		);
	}

	bool ContextWrapper::isInteropAvailable( const Desc::E_INTEROP_API p_api ) const
	{
		return interopAvailability( p_api ).available();
	}

	std::vector<std::byte> ContextWrapper::getTextureData(
		const Desc::Key &			  p_key,
		std::optional<Desc::E_FORMAT> p_format,
		std::optional<size_t>		  p_x,
		std::optional<size_t>		  p_y
	) const
	{
		return std::visit(
			[ & ]( auto & p_backend )
			{
				using T = std::remove_cvref_t<decltype( p_backend )>;
				if constexpr ( not std::is_same_v<T, std::monostate> )
				{
					return p_backend.getTextureData( p_key, p_format, p_x, p_y );
				}
				return std::vector<std::byte> {};
			},
			_impl->backend
		);
	}

	void ContextWrapper::setTextureData( const Desc::Key & p_key, SpanBytes p_bytes )
	{
		std::visit(
			[ & ]( auto & p_backend )
			{
				using T = std::remove_cvref_t<decltype( p_backend )>;
				if constexpr ( not std::is_same_v<T, std::monostate> )
				{
					p_backend.setTextureData( p_key, p_bytes );
				}
			},
			_impl->backend
		);
	}

	void ContextWrapper::setRenderTarget( const Desc::E_RENDER_TARGET p_target )
	{
		std::visit(
			[ & ]( auto & p_backend )
			{
				using T = std::remove_cvref_t<decltype( p_backend )>;
				if constexpr ( not std::is_same_v<T, std::monostate> )
				{
					p_backend.setRenderTarget( p_target );
				}
			},
			_impl->backend
		);
	}

	void ContextWrapper::setOption( const Desc::E_OPTION p_option, const bool p_value )
	{
		std::visit(
			[ & ]( auto & p_backend )
			{
				using T = std::remove_cvref_t<decltype( p_backend )>;
				if constexpr ( not std::is_same_v<T, std::monostate> )
				{
					p_backend.setOption( p_option, p_value );
				}
			},
			_impl->backend
		);
	}

	void ContextWrapper::fillInfos( StructInfos & p_infos ) const
	{
		std::visit(
			[ & ]( auto & p_backend )
			{
				using T = std::remove_cvref_t<decltype( p_backend )>;
				if constexpr ( not std::is_same_v<T, std::monostate> )
				{
					p_backend.fillInfos( p_infos );
				}
			},
			_impl->backend
		);
	}

	const CommandBuffer & ContextWrapper::commands() const { return _impl->commands; }

	void ContextWrapper::clear()
	{
		_impl->commands.clear();
		_impl->backend.emplace<std::monostate>();
		_impl->executor.emplace<std::monostate>();
	}
} // namespace VTX::Renderer::Context
