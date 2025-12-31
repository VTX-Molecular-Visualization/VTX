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
				const auto & p	  = p_commandBuffer.getPayload<PayloadBeginFrame>( command.payloadOffset );
				GLbitfield	 mask = 0;
				if ( p.clearFlags & CLEAR_COLOR )
				{
					mask |= GL_COLOR_BUFFER_BIT;
				}
				if ( p.clearFlags & CLEAR_DEPTH )
				{
					mask |= GL_DEPTH_BUFFER_BIT;
				}
				if ( p.clearFlags & CLEAR_STENCIL )
				{
					mask |= GL_STENCIL_BUFFER_BIT;
				}
				glClear( mask );
				break;
			}
			case E_COMMAND::END_FRAME:
			{
				break;
			}
			case E_COMMAND::BEGIN_PASS:
			{
				const auto & p = p_commandBuffer.getPayload<PayloadBeginPass>( command.payloadOffset );
				_backend.bindFramebuffer( p.renderTarget );
				break;
			}
			case E_COMMAND::END_PASS:
			{
				const auto & p = p_commandBuffer.getPayload<PayloadEndPass>( command.payloadOffset );
				_backend.unbindFramebuffer( p.renderTarget );
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

} // namespace VTX::Renderer::Context::Executor
