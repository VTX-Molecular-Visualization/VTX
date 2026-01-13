#include "renderer/context/executor/opengl45.hpp"

namespace VTX::Renderer::Context::Executor
{

	void OpenGL45::execute( const CommandBuffer & p_commandBuffer ) const noexcept
	{
		for ( const Command & command : p_commandBuffer.commands )
		{
			switch ( command.type )
			{
			case E_COMMAND::BEGIN_PASS:
			{
				const auto & p = p_commandBuffer.getPayload<PayloadBeginPass>( command.payloadOffset );

				// Bind framebuffer.
				const Handle & h = p.framebuffer;
				if ( h == NO_HANDLE )
				{
					GL::Framebuffer::bindDefault();
				}
				else
				{
					_backend.framebuffer( h ).bind();
				}

				// Setting flags.
				const uint32_t flags = p.flags;
				if ( not flags )
				{
					continue;
				}

				// Clear buffers.
				GLbitfield clearMask = 0;
				if ( flags & CLEAR_COLOR )
					clearMask |= GL_COLOR_BUFFER_BIT;
				if ( flags & CLEAR_DEPTH )
					clearMask |= GL_DEPTH_BUFFER_BIT;
				if ( clearMask )
					glClear( clearMask );

				break;
			}
			case E_COMMAND::END_PASS:
			{
				const auto & p = p_commandBuffer.getPayload<PayloadEndPass>( command.payloadOffset );

				// Disable states.
				if ( p.flags & ENABLE_DEPTH )
				{
					glDisable( GL_DEPTH_TEST );
				}

				break;
			}
			}
		}
	}

} // namespace VTX::Renderer::Context::Executor
