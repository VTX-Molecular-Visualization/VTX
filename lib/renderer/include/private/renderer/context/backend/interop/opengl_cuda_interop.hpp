#ifndef __VTX_RENDERER_CONTEXT_BACKEND_INTEROP_OPENGL_CUDA_INTEROP__
#define __VTX_RENDERER_CONTEXT_BACKEND_INTEROP_OPENGL_CUDA_INTEROP__

#include "renderer/context/context_wrapper.hpp"
#include "renderer/descriptors.hpp"
#include <memory>

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
		void					  refreshAvailability();
		Desc::InteropAvailability availability() const;

		/**
		 * @brief Clear all registered buffers.
		 */
		void clear();

		/**
		 * @brief Map/unmap an OpenGL graphics buffer to a CUDA device pointer.
		 */
		Desc::InteropBufferMapping mapBuffer( const Desc::Key &, uint, Desc::E_BUFFER_USAGE );
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
