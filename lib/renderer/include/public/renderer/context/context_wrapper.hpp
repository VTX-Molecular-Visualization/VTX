#ifndef __VTX_RENDERER_CONTEXT_CONTEXT_WRAPPER__
#define __VTX_RENDERER_CONTEXT_CONTEXT_WRAPPER__

#include "renderer/binary_buffer.hpp"
#include "renderer/context/command_buffer.hpp"
#include "renderer/descriptors.hpp"
#include "renderer/struct_infos.hpp"

namespace VTX::Renderer::Context
{
	/**
	 * @brief Wrapper to switch between different backends and executors at runtime.
	 */
	class ContextWrapper
	{
	  public:
		/**
		 * @brief Constructors.
		 */
		ContextWrapper();
		~ContextWrapper();

		/**
		 * @brief Set OpenGL 4.5 graphic context backkend and executor.
		 */
		void setOpenGL45( const size_t, const size_t, const FilePath &, void * = nullptr );

		/**
		 * @brief Set null executor.
		 */
		void setNull();

		/**
		 * @brief Execute the current command buffer.
		 */
		void execute() const noexcept;

		/**
		 * @brief Build the command buffer from the render queue and resources.
		 */
		void build( const Desc::RenderQueue &, const Desc::Resources & );

		/**
		 * @brief Resize backend resources.
		 */
		void resize(
			const uint32_t,
			const uint32_t,
			const Desc::PassList &,
			const std::unordered_map<Desc::Key, Desc::Texture> &
		);

		/**
		 * @brief Convert a span of T to a span of bytes.
		 */
		template<class T>
		static SpanBytes asBytes( std::span<const T> p_s ) noexcept
		{
			static_assert( std::is_trivially_copyable_v<T>, "asWritableBytes(span<T>): T must be trivially copyable." );

			return std::as_bytes( p_s );
		}

		/**
		 * @brief Set shader buffer data.
		 */
		template<class T>
		void setShaderBuffer( const Desc::Key & p_key, std::span<const T> p_data )
		{
			setShaderBuffer( p_key, asBytes( p_data ) );
		}
		void setShaderBuffer( const Desc::Key & p_key, SpanBytes );

		/**
		 * @brief Set pipeline buffer data.
		 */
		template<class T>
		void setPipelineBuffer( const Desc::Key & p_key, std::span<const T> p_data )
		{
			setPipelineBuffer( p_key, asBytes( p_data ) );
		}
		void setPipelineBuffer( const Desc::Key & p_key, SpanBytes );

		/**
		 * @brief Fill renderer infos.
		 */
		void fillInfos( StructInfos & p_infos ) const;

		/**
		 * @brief Getters.
		 */
		const CommandBuffer & commands() const;

		/**
		 * @brief Clean all.
		 */
		void clear();

	  private:
		/**
		 * @brief Pimpl.
		 */
		struct Impl;
		std::unique_ptr<Impl> _impl;
	};

} // namespace VTX::Renderer::Context

#endif
