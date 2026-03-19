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
		void setOpenGL( const size_t p_width, const size_t p_height, const uintptr_t p_nativeWindow, const FilePath & p_shaderDir );

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
		template<typename T>
		static SpanBytes asBytes( std::span<const T> p_span ) noexcept
		{
			static_assert( std::is_trivially_copyable_v<T>, "asWritableBytes(span<T>): T must be trivially copyable." );

			return std::as_bytes( p_span );
		}

		/**
		 * @brief Set shader buffer data.
		 */
		template<typename T>
		inline void setShaderBuffer( const Desc::Key & p_key, std::span<const T> p_data, const size_t p_offset = 0 )
		{
			setShaderBuffer( p_key, asBytes( p_data ), p_offset * sizeof( T ) );
		}
		template<typename T>
		inline void setShaderBuffer( const Desc::Key & p_key, const size_t p_size )
		{
			auto span = SpanBytes { static_cast<std::byte *>( nullptr ), p_size * sizeof( T ) };
			setPipelineBuffer( p_key, span, 0 );
		}
		void setShaderBuffer( const Desc::Key & p_key, SpanBytes, const size_t p_offset = 0 );

		/**
		 * @brief Set pipeline buffer data.
		 */
		template<typename T>
		inline void setPipelineBuffer( const Desc::Key & p_key, std::span<const T> p_data, const size_t p_offset = 0 )
		{
			setPipelineBuffer( p_key, asBytes( p_data ), p_offset * sizeof( T ) );
		}
		template<typename T>
		inline void setPipelineBuffer( const Desc::Key & p_key, const size_t p_size )
		{
			auto span = SpanBytes { static_cast<std::byte *>( nullptr ), p_size * sizeof( T ) };
			setPipelineBuffer( p_key, span, 0 );
		}
		void setPipelineBuffer( const Desc::Key & p_key, SpanBytes, const size_t p_offset = 0 );

		/**
		 * @brief Get texture data.
		 * Can use a different read format (default is upload format).
		 * Coordinates = single pixel, default = whole texture.
		 */
		std::vector<std::byte> getTextureData(
			const Desc::Key & p_key,
			std::optional<Desc::E_FORMAT> = {},
			std::optional<size_t> p_x	  = {},
			std::optional<size_t> p_y	  = {}
		) const;

		/**
		 * @brief Set texture data.
		 */
		template<typename T>
		void setTextureData( const Desc::Key & p_key, std::span<const T> p_data )
		{
			setTextureData( p_key, asBytes( p_data ) );
		}
		void setTextureData( const Desc::Key & p_key, SpanBytes p_data );

		/**
		 * @brief Set the render target.
		 */
		void setRenderTarget( const Desc::E_RENDER_TARGET p_target );

		/**
		 * @brief Set options.
		 */
		void setOption( const Desc::E_OPTION p_option, const bool p_value );

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
