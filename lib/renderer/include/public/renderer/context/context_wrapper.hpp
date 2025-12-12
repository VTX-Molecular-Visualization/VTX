#ifndef __VTX_RENDERER_CONTEXT_CONTEXT_WRAPPER__
#define __VTX_RENDERER_CONTEXT_CONTEXT_WRAPPER__

#include "renderer/context/command_buffer.hpp"
#include "renderer/descriptors.hpp"

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
		void execute();

		/**
		 * @brief Build the command buffer from the render queue and resources.
		 */
		void build( const RenderQueue &, const Resources & );

		/**
		 * @brief Resize backend resources.
		 */
		void resize( const std::size_t, const std::size_t );

		/**
		 * @brief Getters.
		 */
		const CommandBuffer & commands() const;

	  private:
		/**
		 * @brief Pimpl.
		 */
		struct Impl;
		std::unique_ptr<Impl> _impl;
	};

} // namespace VTX::Renderer::Context

#endif
