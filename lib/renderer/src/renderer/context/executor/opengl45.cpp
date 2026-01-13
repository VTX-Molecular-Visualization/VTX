#include "renderer/context/executor/opengl45.hpp"

namespace VTX::Renderer::Context::Executor
{

	void OpenGL45::execute( const CommandBuffer & p_commandBuffer ) const noexcept
	{
		for ( const Command & command : p_commandBuffer.commands )
		{
			switch ( command.type )
			{
			case E_COMMAND::BEGIN_FRAME:
			{
				break;
			}
			case E_COMMAND::END_FRAME:
			{
				break;
			}

			case E_COMMAND::BEGIN_PASS:
			{
				const auto & p = p_commandBuffer.getPayload<PayloadBeginPass>( command.payloadOffset );

				// Clear buffers.
				if ( p.clearFlags )
				{
					glClear( p.clearFlags );
				}

				// Enable states.
				if ( p.enableFlags )
				{
					glEnable( p.enableFlags );
				}

				break;
			}
			case E_COMMAND::END_PASS:
			{
				const auto & p = p_commandBuffer.getPayload<PayloadEndPass>( command.payloadOffset );

				// Disable states.
				if ( p.disableFlags )
				{
					glDisable( p.disableFlags );
				}

				break;
			}
			case E_COMMAND::BIND_FRAMEBUFFER:
			{
				const auto &   p = p_commandBuffer.getPayload<PayloadBindFramebuffer>( command.payloadOffset );
				const Handle & h = p.framebuffer;

				if ( h == NO_HANDLE )
				{
					GL::Framebuffer::bindDefault();
				}
				else
				{
					_backend.framebuffer( h ).bind();
				}

				break;
			}
			case E_COMMAND::BIND_TEXTURE:
			{
				break;
			}
			case E_COMMAND::BIND_BUFFER:
			{
				break;
			}
			}
		}
	}

} // namespace VTX::Renderer::Context::Executor
