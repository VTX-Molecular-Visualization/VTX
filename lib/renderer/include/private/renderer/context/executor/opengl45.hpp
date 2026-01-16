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
			using namespace Desc;

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

					// Enable states.
					if ( flags & ENABLE_DEPTH )
					{
						glEnable( GL_DEPTH_TEST );
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
				case E_COMMAND::DRAW_ARRAY:
				{
					const auto & p = p_commandBuffer.getPayload<PayloadDrawArray>( command.payloadOffset );

					_backend.vertexArray( p.pipeline ).bind();
					_backend.program( p.program ).use();

					if ( p.vertexCount )
					{
						_backend.vertexArray( p.pipeline ).drawArray( p.primitive, 0, p.vertexCount );
					}

					break;
				}
				case E_COMMAND::DRAW_ELEMENT:
				{
					const auto & p = p_commandBuffer.getPayload<PayloadDrawElement>( command.payloadOffset );

					_backend.vertexArray( p.pipeline ).bind();
					_backend.program( p.program ).use();

					if ( p.indexCount )
					{
						_backend.vertexArray( p.pipeline ).drawElement( p.primitive, p.indexCount, GL_UNSIGNED_INT, 0 );
					}

					break;
				}
				case E_COMMAND::DRAW_ARRAYS:
				{
					const auto & p		= p_commandBuffer.getPayload<PayloadDrawArrays>( command.payloadOffset );
					const auto * ranges = reinterpret_cast<DrawCall::RangeArrays *>( p.vertexRanges );

					_backend.vertexArray( p.pipeline ).bind();
					_backend.program( p.program ).use();

					if ( ranges && ranges->counts.size() > 0 )
					{
						assert( ranges->counts.size() == ranges->firsts.size() );

						_backend.vertexArray( p.pipeline )
							.multiDrawArray(
								p.primitive,
								reinterpret_cast<const GLint *>( ranges->firsts.data() ),
								reinterpret_cast<const GLsizei *>( ranges->counts.data() ),
								static_cast<GLsizei>( ranges->counts.size() )
							);
					}

					break;
				}
				case E_COMMAND::DRAW_ELEMENTS:
				{
					const auto & p		= p_commandBuffer.getPayload<PayloadDrawElements>( command.payloadOffset );
					const auto * ranges = reinterpret_cast<DrawCall::RangeElements *>( p.indexRanges );

					_backend.vertexArray( p.pipeline ).bind();
					_backend.program( p.program ).use();

					if ( ranges && ranges->counts.size() > 0 )
					{
						assert( ranges->counts.size() == ranges->offsets.size() );

						_backend.vertexArray( p.pipeline )
							.multiDrawElement(
								p.primitive,
								reinterpret_cast<const GLsizei *>( ranges->counts.data() ),
								GL_UNSIGNED_INT,
								reinterpret_cast<const GLvoid * const *>( ranges->offsets.data() ),
								static_cast<GLsizei>( ranges->counts.size() )
							);
					}

					break;
				}
				default: break;
				}
			}
		}

	  private:
		const Backend::OpenGL45 & _backend;
	};
} // namespace VTX::Renderer::Context::Executor

#endif
