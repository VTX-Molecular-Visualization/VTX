#include "renderer/context/context_wrapper.hpp"
#include "renderer/context/backend/opengl.hpp"
#include "renderer/context/executor/null.hpp"
#include "renderer/context/executor/opengl.hpp"
#include <type_traits>
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
	} // namespace

	ContextWrapper::ContextWrapper() : _impl( std::make_unique<Impl>() ) {}
	ContextWrapper::~ContextWrapper() = default;

	void ContextWrapper::setOpenGL(
		const size_t	 p_width,
		const size_t	 p_height,
		const uintptr_t	 p_nativeWindow,
		const FilePath & p_shaderDir
	)
	{
		_setBackend<Backend::OpenGL>(
			_impl->backend, _impl->executor, p_width, p_height, p_nativeWindow, p_shaderDir
		);
		_setExecutor<Executor::OpenGL>( _impl->backend, _impl->executor );
	}

	void ContextWrapper::setNull()
	{
		_setBackend<std::monostate>( _impl->backend, _impl->executor );
		_setExecutor<Executor::Null>( _impl->backend, _impl->executor );
	}

	void ContextWrapper::execute() const noexcept
	{
		std::visit(
			[ & ]( const auto & p_exec )
			{
				using T = std::remove_cvref_t<decltype( p_exec )>;
				if constexpr ( not std::is_same_v<T, std::monostate> )
				{
					p_exec.execute( _impl->commands );
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

	void ContextWrapper::setShaderBuffer( const Desc::Key & p_key, SpanBytes p_bytes, const size_t p_offset )
	{
		if ( p_bytes.size() == 0 )
		{
			return;
		}

		std::visit(
			[ & ]( auto & p_backend )
			{
				using T = std::remove_cvref_t<decltype( p_backend )>;
				if constexpr ( not std::is_same_v<T, std::monostate> )
				{
					p_backend.setShaderBufferData( p_key, p_bytes, p_offset );
				}
			},
			_impl->backend
		);
	}

	void ContextWrapper::setPipelineBuffer( const Desc::Key & p_key, SpanBytes p_bytes, const size_t p_offset )
	{
		if ( p_bytes.size() == 0 )
		{
			return;
		}

		std::visit(
			[ & ]( auto & p_backend )
			{
				using T = std::remove_cvref_t<decltype( p_backend )>;
				if constexpr ( not std::is_same_v<T, std::monostate> )
				{
					p_backend.setPipelineBufferData( p_key, p_bytes, p_offset );
				}
			},
			_impl->backend
		);
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
