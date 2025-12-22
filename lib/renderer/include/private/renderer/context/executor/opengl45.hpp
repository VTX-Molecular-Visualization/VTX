#ifndef __VTX_RENDERER_CONTEXT_EXECUTOR_OPENGL45__
#define __VTX_RENDERER_CONTEXT_EXECUTOR_OPENGL45__

#include "renderer/context/backend/opengl45.hpp"
#include "renderer/context/command_buffer.hpp"

namespace VTX::Renderer::Context::Executor
{
	/**
	 * @brief OpenGL 4.5 executor.
	 */
	class OpenGL45
	{
	  public:
		/**
		 * @brief Construct from backend.
		 */
		OpenGL45( const Backend::OpenGL45 & p_backend ) : _backend( p_backend )
		{
			glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
		}

		/**
		 * @brief Execute commands from the command buffer.
		 */
		inline void execute( const CommandBuffer & ) const noexcept
		{
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		}

	  private:
		const Backend::OpenGL45 & _backend;
	};
} // namespace VTX::Renderer::Context::Executor

#endif
