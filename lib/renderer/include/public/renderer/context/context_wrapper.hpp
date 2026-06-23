#ifndef __VTX_RENDERER_CONTEXT_CONTEXT_WRAPPER__
#define __VTX_RENDERER_CONTEXT_CONTEXT_WRAPPER__

#include "renderer/binary_buffer.hpp"
#include "renderer/context/command_buffer.hpp"
#include "renderer/descriptors.hpp"
#include "renderer/struct_infos.hpp"
#include <functional>
#include <memory>
#include <span>
#include <vector>

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
		void setOpenGL(
			const size_t p_width,
			const size_t p_height,
			const Desc::NativeContextInfo &,
			const FilePath & p_shaderDir
		);

		/**
		 * @brief Set null executor.
		 */
		void setNull();

		/**
		 * @brief Execute the current command buffer.
		 */
		void execute() const noexcept;

		/**
		 * @brief Execute selected pass ranges from the current command buffer.
		 */
		void execute( std::span<const PassID> ) const noexcept;

		/**
		 * @brief Build the command buffer from the render queue and resources.
		 */
		void build( const Desc::RenderQueue &, const Desc::Resources & );

		/**
		 * @brief Set the function and user context for an external pass command.
		 */
		void setExternalPass( const Desc::Key &, uintptr_t, uintptr_t = 0 );

		/**
		 * @brief Check if a pass exists in the current command buffer.
		 */
		bool containsPass( const Desc::Key & ) const;

		/**
		 * @brief Schedule an ON_DIRTY pass for the next execute().
		 */
		void markPassDirty( const Desc::Key & );

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
		 * @brief Set buffer data.
		 */
		template<typename T>
		inline void setBuffer( const Desc::BufferRef & p_ref, std::span<const T> p_data, const size_t p_offset = 0 )
		{ setBuffer( p_ref, asBytes( p_data ), p_offset * sizeof( T ) ); }

		template<typename T>
		inline void setBuffer( const Desc::BufferRef & p_ref, const size_t p_size )
		{
			auto span = SpanBytes { static_cast<std::byte *>( nullptr ), p_size * sizeof( T ) };
			setBuffer( p_ref, span, 0 );
		}

		void setBuffer( const Desc::BufferRef &, SpanBytes, const size_t p_offset = 0 );

		/**
		 * @brief Ensure a physical chunk exists for a chunked logical buffer.
		 * @return true if a new
		 * chunk was created and the command buffer must be rebuilt.
		 */
		bool ensureBufferChunk( const Desc::BufferRef & );

		/**
		 * @brief Release a physical chunk for a chunked logical buffer.
		 * @return true if a chunk
		 * was released and the command buffer must be rebuilt.
		 */
		bool releaseBufferChunk( const Desc::BufferRef & );

		/**
		 * @brief Get texture data.
		 * Can use a different read format (default is upload format).

		 * * Coordinates = single pixel, default = whole texture.
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
		{ setTextureData( p_key, asBytes( p_data ) ); }

		template<typename T>
		void setTextureData(
			const Desc::Key &  p_key,
			std::span<const T> p_data,
			const size_t	   p_width,
			const size_t	   p_height
		)
		{
			setTextureData(
				p_key, asBytes( p_data ), std::optional<size_t> { p_width }, std::optional<size_t> { p_height }
			);
		}

		void setTextureData(
			const Desc::Key &	  p_key,
			SpanBytes			  p_data,
			std::optional<size_t> p_width  = {},
			std::optional<size_t> p_height = {}
		);

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
		 * @brief Map graphics buffers to an external compute backend pointer.
		 */
		[[nodiscard]] Desc::InteropBufferMapping mapInteropBuffer( const Desc::E_INTEROP_API, const Desc::BufferRef & );
		[[nodiscard]] std::vector<Desc::InteropBufferMapping> mapInteropBuffers(
			const Desc::E_INTEROP_API,
			std::span<const Desc::BufferRef>
		);
		void unmapInteropBuffer( const Desc::E_INTEROP_API, const Desc::InteropBufferMapping & );
		void unmapInteropBuffers( const Desc::E_INTEROP_API, std::span<const Desc::InteropBufferMapping> );
		void unregisterInteropBuffer( Desc::E_INTEROP_API, const Desc::BufferRef & );

		/**
		 * @brief Query whether a backend interop API can access renderer-owned graphics resources.

		 */
		Desc::InteropAvailability interopAvailability( Desc::E_INTEROP_API ) const;
		bool					  isInteropAvailable( Desc::E_INTEROP_API ) const;

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
