#ifndef __VTX_RENDERER_CONTEXT_BACKEND_INTEROP_OPENGL_CUDA_INTEROP__
#define __VTX_RENDERER_CONTEXT_BACKEND_INTEROP_OPENGL_CUDA_INTEROP__

#include "renderer/context/context_wrapper.hpp"
#include "renderer/descriptors.hpp"
#include <memory>
#include <span>
#include <vector>

namespace VTX::Renderer::Context::Backend::Interop
{
	/**
	 * @brief OpenGL-CUDA interop helper.
	 */
	class OpenGLCudaInterop
	{
	  public:
		/**
		 * @brief Constructors.
		 */
		OpenGLCudaInterop();
		~OpenGLCudaInterop();

		/**
		 * @brief Availability.
		 */
		void					  refreshAvailability( uint p_probeBuffer );
		Desc::InteropAvailability availability() const;

		/**
		 * @brief Clear all registered buffers.
		 */
		void clear();

		/**
		 * @brief Map/unmap an OpenGL graphics buffer to a CUDA device pointer.
		 */
		struct BufferRequest
		{
			Desc::Key			 key;
			uint				 graphicsBufferId = 0;
			Desc::E_BUFFER_USAGE usage			  = Desc::E_BUFFER_USAGE::NONE;
		};

		Desc::InteropBufferMapping mapBuffer( const Desc::Key &, uint, Desc::E_BUFFER_USAGE );
		std::vector<Desc::InteropBufferMapping> mapBuffers( std::span<const BufferRequest> );
		void					   unmapBuffer( const Desc::Key & );

		/**
		 * @brief Unregister a buffer when a buffer is invalidated (resized for example).
		 */
		void unregisterBuffer( const Desc::Key & );

	  private:
		/**
		 * @brief Pimpl for CUDA isolation.
		 */
		struct Impl;
		std::unique_ptr<Impl> _impl;
	};
} // namespace VTX::Renderer::Context::Backend::Interop

#endif
