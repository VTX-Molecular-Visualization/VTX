#include "renderer/context/executor/opengl45.hpp"
#include "renderer/context/gl/debug.hpp"

namespace
{
	using namespace VTX;
	using namespace VTX::Renderer;

	constexpr GLenum _toGL( const uint32_t p_primitive ) noexcept
	{
		using namespace Desc;

		E_PRIMITIVE p = static_cast<E_PRIMITIVE>( p_primitive );

		switch ( p )
		{
		case E_PRIMITIVE::POINTS: return GL_POINTS;
		case E_PRIMITIVE::LINES: return GL_LINES;
		case E_PRIMITIVE::TRIANGLES: return GL_TRIANGLE_STRIP;
		case E_PRIMITIVE::PATCHES: return GL_PATCHES;
		default: assert( false ); return GL_INVALID_INDEX;
		}
	}

} // namespace

namespace VTX::Renderer::Context::Executor
{

	OpenGL45::OpenGL45( const Backend::OpenGL45 & p_backend ) : _backend( p_backend ) {}

	void OpenGL45::execute( const CommandBuffer & p_commandBuffer ) const noexcept
	{
		using namespace Desc;

		for ( const Command & command : p_commandBuffer.commands )
		{
#ifdef _DEBUG
			GL::Debug::dumpGLError();
#endif

			switch ( command.type )
			{
			case E_COMMAND::BEGIN_PASS:
			{
				const auto & p = p_commandBuffer.getPayload<PayloadBeginPass>( command.payloadOffset );

				// Bind framebuffer.
				_backend.framebuffer( p.framebuffer ).bind();

				// Setting flags.
				const uint32_t flags = p.flags;
				if ( not flags )
				{
					continue;
				}

				// Enable states.
				if ( flags & toUnderlying( E_SETTING::ENABLE_DEPTH ) )
				{
					glEnable( GL_DEPTH_TEST );
				}

				// Clear buffers.
				GLbitfield clearMask = 0;
				if ( flags & toUnderlying( E_SETTING::CLEAR_COLOR ) )
					clearMask |= GL_COLOR_BUFFER_BIT;
				if ( flags & toUnderlying( E_SETTING::CLEAR_DEPTH ) )
					clearMask |= GL_DEPTH_BUFFER_BIT;
				if ( clearMask )
					glClear( clearMask );

				break;
			}
			case E_COMMAND::END_PASS:
			{
				const auto & p = p_commandBuffer.getPayload<PayloadEndPass>( command.payloadOffset );

				// Disable states.
				if ( p.flags & toUnderlying( E_SETTING::ENABLE_DEPTH ) )
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
				assert( p.count > 0 );

				_backend.vertexArray( p.pipeline ).bind();
				_backend.program( p.program ).use();
				_backend.vertexArray( p.pipeline )
					.drawArrays(
						_toGL( p.primitive ), static_cast<uint32_t>( p.first ), static_cast<uint32_t>( p.count )
					);

				break;
			}
			case E_COMMAND::DRAW_INDEXED:
			{
				const auto & p = p_commandBuffer.getPayload<PayloadDrawIndexed>( command.payloadOffset );
				assert( p.count > 0 );

				_backend.vertexArray( p.pipeline ).bind();
				_backend.program( p.program ).use();
				_backend.vertexArray( p.pipeline )
					.drawElements( _toGL( p.primitive ), static_cast<uint32_t>( p.count ), GL_UNSIGNED_INT, &p.first );

				break;
			}
			case E_COMMAND::DRAW_INDIRECT:
			{
				const auto &	 p	   = p_commandBuffer.getPayload<PayloadDrawIndirect>( command.payloadOffset );
				const uint32_t * count = reinterpret_cast<uint32_t *>( p.count );

				_backend.vertexArray( p.pipeline ).bind();
				_backend.pipelineBuffer( p.buffer ).bind( GL_DRAW_INDIRECT_BUFFER );
				_backend.program( p.program ).use();
				_backend.vertexArray( p.pipeline ).multiDrawArraysIndirect( _toGL( p.primitive ), nullptr, *count );

				break;
			}
			case E_COMMAND::DRAW_INDEXED_INDIRECT:
			{
				const auto &	 p = p_commandBuffer.getPayload<PayloadDrawIndexedIndirect>( command.payloadOffset );
				const uint32_t * count = reinterpret_cast<uint32_t *>( p.count );

				_backend.vertexArray( p.pipeline ).bind();
				_backend.pipelineBuffer( p.buffer ).bind( GL_DRAW_INDIRECT_BUFFER );
				_backend.program( p.program ).use();
				_backend.vertexArray( p.pipeline )
					.multiDrawElementsIndirect( _toGL( p.primitive ), GL_UNSIGNED_INT, nullptr, *count );

				break;
			}
			case E_COMMAND::BIND_OUTPUT:
			{
				const auto & p = p_commandBuffer.getPayload<PayloadBindOutput>( command.payloadOffset );
				// Bind output framebuffer.
				const Handle hFramebuffer = *reinterpret_cast<Handle *>( p.framebuffer );
				_backend.framebuffer( hFramebuffer ).bind();
				break;
			}

			default: break;
			}
		}

#ifdef _DEBUG
		GL::Debug::dumpGLError();
#endif
	}

} // namespace VTX::Renderer::Context::Executor
