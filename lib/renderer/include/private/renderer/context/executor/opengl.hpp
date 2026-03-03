#ifndef __VTX_RENDERER_CONTEXT_EXECUTOR_OPENGL45__
#define __VTX_RENDERER_CONTEXT_EXECUTOR_OPENGL45__

#include "renderer/context/backend/opengl.hpp"
#include "renderer/context/command_buffer.hpp"

namespace VTX::Renderer::Context::Executor
{
	/**
	 * @brief OpenGL executor.
	 */
	class OpenGL
	{
	  public:
		/**
		 * @brief Construct from backend.
		 */
		OpenGL( const Backend::OpenGL & );

		/**
		 * @brief Execute commands from the command buffer.
		 */
		void execute( const CommandBuffer & ) const noexcept;

	  private:
		const Backend::OpenGL & _backend;
	};
} // namespace VTX::Renderer::Context::Executor

#endif
