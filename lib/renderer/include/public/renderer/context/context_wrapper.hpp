#ifndef __VTX_RENDERER_CONTEXT_CONTEXT_WRAPPER__
#define __VTX_RENDERER_CONTEXT_CONTEXT_WRAPPER__

#include "renderer/context/backend/opengl45.hpp"
#include "renderer/context/command_buffer.hpp"
#include "renderer/context/executor/null.hpp"
#include "renderer/context/executor/opengl45.hpp"
#include "renderer/descriptors.hpp"
#include <type_traits>
#include <utility>
#include <variant>

namespace VTX::Renderer::Context
{
	/**
	 * @brief Wrapper to switch between different backends and executors at runtime.
	 */
	class ContextWrapper
	{
	  public:
		/**
		 * @brief Available backends and executors.
		 */
		using BackendVariant  = std::variant<std::monostate, Backend::OpenGL45>;
		using ExecutorVariant = std::variant<std::monostate, Executor::Null, Executor::OpenGL45>;

		/**
		 * @brief Set the graphic backend.
		 */
		template<typename B, typename... Args>
		void setBackend( Args &&... p_args )
		{
			_backend.emplace<B>( std::forward<Args>( p_args )... );

			// Rebuild executor to match the new backend.
			if constexpr ( std::is_same_v<B, Backend::OpenGL45> )
			{
				if ( auto * executor = std::get_if<Executor::OpenGL45>( &_executor ) )
				{
					*executor = Executor::OpenGL45( std::get<Backend::OpenGL45>( _backend ) );
				}
			}
		}

		/**
		 * @brief Set the command buffer executor.
		 */
		template<typename E>
		void setExecutor()
		{
			if constexpr ( std::is_same_v<E, Executor::OpenGL45> )
			{
				if ( auto * backend = std::get_if<Backend::OpenGL45>( &_backend ) )
				{
					_executor.emplace<Executor::OpenGL45>( *backend );
					return;
				}
			}

			_executor.emplace<Executor::Null>();
		}

		/**
		 * @brief Execute the current command buffer.
		 */
		inline void execute()
		{
			std::visit(
				[ & ]( auto & p_exec )
				{
					using T = std::decay_t<decltype( p_exec )>;
					if constexpr ( not std::is_same_v<T, std::monostate> )
					{
						p_exec.execute( _commands );
					}
				},
				_executor
			);
		}

		/**
		 * @brief Build the command buffer from the render queue and resources.
		 */
		inline void build( const RenderQueue & p_renderQueue, const Resources & p_resources )
		{
			_commands.clear();

			std::visit(
				[ & ]( auto & p_backend )
				{
					using T = std::decay_t<decltype( p_backend )>;
					if constexpr ( not std::is_same_v<T, std::monostate> )
					{
						p_backend.build( p_renderQueue, p_resources, _commands );
					}
				},
				_backend
			);
		}

		/**
		 * @brief Resize backend resources.
		 */
		inline void resize( const std::size_t p_width, const std::size_t p_height )
		{
			std::visit(
				[ & ]( auto & p_backend )
				{
					using T = std::decay_t<decltype( p_backend )>;
					if constexpr ( not std::is_same_v<T, std::monostate> )
					{
						// p_backend.resize( p_width, p_height );
					}
				},
				_backend
			);
		}

		/**
		 * @brief Convenience functions.
		 */
		inline bool hasBackend() const { return not std::holds_alternative<std::monostate>( _backend ); }
		inline bool hasExecutor() const { return not std::holds_alternative<std::monostate>( _executor ); }
		inline const CommandBuffer & commands() const { return _commands; }
		inline CommandBuffer &		 commands() { return _commands; }

	  private:
		/**
		 * @brief Current backend.
		 */
		BackendVariant _backend;

		/**
		 * @brief Current executor.
		 */
		ExecutorVariant _executor;

		/**
		 * @brief Command buffer to fill and execute.
		 */
		CommandBuffer _commands;
	};

} // namespace VTX::Renderer::Context

#endif
