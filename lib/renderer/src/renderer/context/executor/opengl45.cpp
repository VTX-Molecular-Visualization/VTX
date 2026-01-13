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
			case E_COMMAND::BIND_RESOURCES:
			{
				const auto & p = p_commandBuffer.getPayload<PayloadBindResources>( command.payloadOffset );

				const Handle							 hResourceTable = p.resourceTable;
				const Backend::OpenGL45::ResourceTable & rt				= _backend.resourceTable( hResourceTable );

				// Textures / samplers.
				for ( const auto & textureBinding : rt.textures )
				{
					const GL::Texture2D & texture = _backend.texture( textureBinding.texture );
					const GL::Sampler &	  sampler = _backend.sampler( textureBinding.sampler );
					const Binding		  unit	  = textureBinding.unit;

					texture.bindToUnit( unit );
					sampler.bindToUnit( unit );
				}

				// Shader buffers.
				for ( const auto & bufferBinding : rt.shaderBuffers )
				{
					const GL::Buffer & buffer = _backend.shaderBuffer( bufferBinding.buffer );
					buffer.bind(
						bufferBinding.kind == E_SHADER_BUFFER_KIND::PARAMETERS ? GL_UNIFORM_BUFFER
																			   : GL_SHADER_STORAGE_BUFFER,
						bufferBinding.binding
					);
				}

				break;
			}
			case E_COMMAND::DRAW:
			{
				const auto & p = p_commandBuffer.getPayload<PayloadDraw>( command.payloadOffset );

				_backend.vertexArray( p.pipeline ).bind();
				_backend.program( p.program ).use();
				switch ( p.primitive )
				{
				case E_PRIMITIVE::POINTS:
					_backend.vertexArray( p.pipeline ).drawArray( GL_POINTS, 0, p.vertexCount );
					break;
				case E_PRIMITIVE::LINES:
					_backend.vertexArray( p.pipeline ).drawArray( GL_LINES, 0, p.vertexCount );
					break;

				case E_PRIMITIVE::TRIANGLES:
					_backend.vertexArray( p.pipeline ).drawArray( GL_TRIANGLES, 0, p.vertexCount );
					break;

				default: break;
				}
			}
			}
		}
	}
} // namespace VTX::Renderer::Context::Executor
