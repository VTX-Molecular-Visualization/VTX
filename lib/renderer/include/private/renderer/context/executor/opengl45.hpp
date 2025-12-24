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
		OpenGL45( const Backend::OpenGL45 & p_backend ) : _backend( p_backend ) {}

		/**
		 * @brief Execute commands from the command buffer.
		 */
		inline void execute( const CommandBuffer & p_commandBuffer ) const noexcept
		{
			for ( const Command & command : p_commandBuffer.commands )
			{
				switch ( command.type )
				{
				case E_COMMAND::CLEAR:
				{
					glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
					break;
				}
				case E_COMMAND::BEGIN_PASS:
				{
					break;
				}
				case E_COMMAND::END_PASS:
				{
					break;
				}
				case E_COMMAND::SET_VIEWPORT:
				{
					break;
				}
				case E_COMMAND::BIND_PIPELINE:
				{
					break;
				}
				case E_COMMAND::BIND_RESOURCE_TABLE:
				{
					break;
				}
				case E_COMMAND::PUSH_CONSTANTS:
				{
					break;
				}
				case E_COMMAND::BIND_GEOMETRY:
				{
					break;
				}
				case E_COMMAND::DRAW:
				{
					break;
				}
				case E_COMMAND::DRAW_INDEXED:
				{
					break;
				}
				case E_COMMAND::DRAW_INDIRECT:
				{
					break;
				}
				case E_COMMAND::DRAW_INDEXED_INDIRECT:
				{
					break;
				}
				case E_COMMAND::DISPATCH:
				{
					break;
				}
				case E_COMMAND::BARRIER:
				{
					break;
				}
				}
			}
		}

	  private:
		const Backend::OpenGL45 & _backend;
	};
} // namespace VTX::Renderer::Context::Executor

#endif
