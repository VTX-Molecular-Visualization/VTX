#ifndef __VTX_RENDERER_CONTEXT_BACKEND_OPENGL45__
#define __VTX_RENDERER_CONTEXT_BACKEND_OPENGL45__

#include "renderer/context/command_buffer.hpp"
#include "renderer/descriptors.hpp"

namespace VTX::Renderer::Context::Backend
{
	/**
	 * @brief OpenGL 4.5 backend.
	 */
	class OpenGL45
	{
	  public:
		/**
		 * @brief Default constructor.
		 */
		OpenGL45() = default;

		/**
		 * @brief Build the command buffer from the render queue and resources.
		 */
		void build( const RenderQueue & p_renderQueue, const Resources & p_resources, CommandBuffer & p_commands ) {}
	};
} // namespace VTX::Renderer::Context::Backend

#endif
