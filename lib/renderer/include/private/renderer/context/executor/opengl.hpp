#ifndef __VTX_RENDERER_CONTEXT_EXECUTOR_OPENGL45__
#define __VTX_RENDERER_CONTEXT_EXECUTOR_OPENGL45__

#include "renderer/context/backend/gl/debug.hpp"
#include "renderer/context/backend/opengl.hpp"
#include "renderer/context/command_buffer.hpp"
#include <util/hashing.hpp>
#include <util/type_traits.hpp>

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
		OpenGL( const Backend::OpenGL & p_backend ) : _backend( p_backend ) {}

		/**
		 * @brief Execute a single command.
		 */
		template<typename PayLoad>
		void execute( const PayLoad & ) const noexcept
		{
			// Display an error with payload type.
			VTX_ERROR( "OpenGL::execute: unknown payload type {}", Util::typeName<PayLoad>() );
		}

		void execute( const PayloadBeginPass & p_payload ) const noexcept
		{
			using namespace Desc;

			// Bind framebuffer.
			_backend.framebuffer( p_payload.framebuffer ).bind();

			// Setting flags.
			const uint32_t flags = p_payload.flags;
			if ( not flags )
			{
				return;
			}

			// Enable states.
			if ( flags & toUnderlying( E_SETTING::ENABLE_DEPTH ) )
			{
				glEnable( GL_DEPTH_TEST );
			}

			// Clear buffers.
			GLbitfield clearMask = 0;
			if ( flags & toUnderlying( E_SETTING::CLEAR_COLOR ) )
			{
				clearMask |= GL_COLOR_BUFFER_BIT;
			}
			if ( flags & toUnderlying( E_SETTING::CLEAR_DEPTH ) )
			{
				clearMask |= GL_DEPTH_BUFFER_BIT;
			}
			if ( clearMask )
			{
				glClear( clearMask );
			}

			_dumpGLError();
		}

		void execute( const PayloadBindResources & p_payload ) const noexcept
		{
			using namespace Desc;

			const Handle						   hResourceTable = p_payload.resourceTable;
			const Backend::OpenGL::ResourceTable & rt			  = _backend.resourceTable( hResourceTable );

			// Textures / samplers.
			for ( const auto & textureBinding : rt.textures )
			{
				const Backend::GL::Texture2D & texture = _backend.texture( textureBinding.texture );
				const Backend::GL::Sampler &   sampler = _backend.sampler( textureBinding.sampler );
				const Binding				   unit	   = textureBinding.unit;

				texture.bindToUnit( unit );
				sampler.bindToUnit( unit );
			}

			// Shader-visible buffers.
			for ( const auto & bufferBinding : rt.buffers )
			{
				const Backend::GL::Buffer & buffer = _backend.buffer( bufferBinding.buffer );
				buffer.bind( _toGLShaderTarget( bufferBinding.usage ), bufferBinding.binding );
			}

			_dumpGLError();
		}

		void execute( const PayloadDraw & p_payload ) const noexcept
		{
			assert( p_payload.count > 0 );

			_backend.vertexArray( p_payload.pipeline ).bind();
			_backend.program( p_payload.program ).use();
			_backend.vertexArray( p_payload.pipeline )
				.drawArrays(
					_toGL( p_payload.primitive ),
					static_cast<uint32_t>( p_payload.first ),
					static_cast<uint32_t>( p_payload.count )
				);

			_dumpGLError();
		}

		void execute( const PayloadDrawIndexed & p_payload ) const noexcept
		{
			assert( p_payload.count > 0 );

			auto & vao = _backend.vertexArray( p_payload.pipeline );
			vao.bind();
			vao.bindElementBuffer( _backend.buffer( p_payload.indexBuffer ) );
			_backend.program( p_payload.program ).use();
			_backend.vertexArray( p_payload.pipeline )
				.drawElements(
					_toGL( p_payload.primitive ),
					static_cast<uint32_t>( p_payload.count ),
					GL_UNSIGNED_INT,
					&p_payload.first
				);

			_dumpGLError();
		}

		void execute( const PayloadDrawIndirect & p_payload ) const noexcept
		{
			const Backend::GL::Buffer & indirectBuffer = _backend.buffer( p_payload.indirectBuffer );
			const GLsizei drawCapacity
				= _drawCapacity( indirectBuffer, p_payload.commandOffset, p_payload.commandStride );

			if ( drawCapacity == 0 )
			{
				return;
			}

			_backend.vertexArray( p_payload.pipeline ).bind();
			indirectBuffer.bind( GL_DRAW_INDIRECT_BUFFER );
			indirectBuffer.bind( GL_PARAMETER_BUFFER );
			_backend.program( p_payload.program ).use();
			_backend.vertexArray( p_payload.pipeline )
				.multiDrawArraysIndirectCount(
					_toGL( p_payload.primitive ),
					reinterpret_cast<const void *>( uintptr_t( p_payload.commandOffset ) ),
					p_payload.countOffset,
					drawCapacity,
					static_cast<GLsizei>( p_payload.commandStride )
				);

			_dumpGLError();
		}

		void execute( const PayloadDrawIndexedIndirect & p_payload ) const noexcept
		{
			const Backend::GL::Buffer & indirectBuffer = _backend.buffer( p_payload.indirectBuffer );
			const GLsizei drawCapacity
				= _drawCapacity( indirectBuffer, p_payload.commandOffset, p_payload.commandStride );

			if ( drawCapacity == 0 )
			{
				return;
			}

			auto & vao = _backend.vertexArray( p_payload.pipeline );
			vao.bind();
			vao.bindElementBuffer( _backend.buffer( p_payload.indiceBuffer ) );
			indirectBuffer.bind( GL_DRAW_INDIRECT_BUFFER );
			indirectBuffer.bind( GL_PARAMETER_BUFFER );
			_backend.program( p_payload.program ).use();
			_backend.vertexArray( p_payload.pipeline )
				.multiDrawElementsIndirectCount(
					_toGL( p_payload.primitive ),
					GL_UNSIGNED_INT,
					reinterpret_cast<const void *>( uintptr_t( p_payload.commandOffset ) ),
					p_payload.countOffset,
					drawCapacity,
					static_cast<GLsizei>( p_payload.commandStride )
				);

			_dumpGLError();
		}

		void execute( const PayloadDispatch & p_payload ) const noexcept
		{
			_backend.program( p_payload.program ).use();
			glDispatchCompute( p_payload.groupX, p_payload.groupY, p_payload.groupZ );
			_applyMemoryBarrier( p_payload.barriers );

			_dumpGLError();
		}

		void execute( const PayloadDispatchIndirect & p_payload ) const noexcept
		{
			_backend.buffer( p_payload.indirectBuffer ).bind( GL_DISPATCH_INDIRECT_BUFFER );
			_backend.program( p_payload.program ).use();
			glDispatchComputeIndirect( p_payload.offset );
			_applyMemoryBarrier( p_payload.barriers );

			_dumpGLError();
		}

		void execute( const PayloadExternal & p_payload ) const noexcept
		{
			if ( p_payload.function == 0 )
			{
				return;
			}

			using ExternalFunction = void ( * )( uintptr_t );
			auto function		   = reinterpret_cast<ExternalFunction>( p_payload.function );
			function( p_payload.context );
		}

		void execute( const PayloadBindOutput & p_payload ) const noexcept
		{
			// Bind output framebuffer.
			_backend.framebuffer( p_payload.framebuffer ).bind();

			_dumpGLError();
		}

		void execute( const PayloadEndPass & p_payload ) const noexcept
		{
			using namespace Desc;

			// Disable states.
			if ( p_payload.flags & toUnderlying( E_SETTING::ENABLE_DEPTH ) )
			{
				glDisable( GL_DEPTH_TEST );
			}

			_dumpGLError();
		}

		void execute( const PayloadPresent & ) const noexcept
		{
			// Swap buffers.
			_backend.swap();

			_dumpGLError();
		}

	  private:
		static void _applyMemoryBarrier( const uint32_t p_barriers ) noexcept
		{
			const GLbitfield barriers = _toGLMemoryBarrier( p_barriers );
			if ( barriers != 0 )
			{
				glMemoryBarrier( barriers );
			}
		}

		static constexpr GLbitfield _toGLMemoryBarrier( const uint32_t p_barriers ) noexcept
		{
			using namespace Desc;

			if ( p_barriers == toUnderlying( E_MEMORY_BARRIER::NONE ) )
			{
				return 0;
			}
			if ( p_barriers == toUnderlying( E_MEMORY_BARRIER::ALL ) )
			{
				return GL_ALL_BARRIER_BITS;
			}

			GLbitfield result = 0;
			if ( p_barriers & toUnderlying( E_MEMORY_BARRIER::VERTEX_ATTRIB_ARRAY ) )
			{
				result |= GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT;
			}
			if ( p_barriers & toUnderlying( E_MEMORY_BARRIER::ELEMENT_ARRAY ) )
			{
				result |= GL_ELEMENT_ARRAY_BARRIER_BIT;
			}
			if ( p_barriers & toUnderlying( E_MEMORY_BARRIER::UNIFORM ) )
			{
				result |= GL_UNIFORM_BARRIER_BIT;
			}
			if ( p_barriers & toUnderlying( E_MEMORY_BARRIER::TEXTURE_FETCH ) )
			{
				result |= GL_TEXTURE_FETCH_BARRIER_BIT;
			}
			if ( p_barriers & toUnderlying( E_MEMORY_BARRIER::SHADER_IMAGE_ACCESS ) )
			{
				result |= GL_SHADER_IMAGE_ACCESS_BARRIER_BIT;
			}
			if ( p_barriers & toUnderlying( E_MEMORY_BARRIER::COMMAND ) )
			{
				result |= GL_COMMAND_BARRIER_BIT;
			}
			if ( p_barriers & toUnderlying( E_MEMORY_BARRIER::PIXEL_BUFFER ) )
			{
				result |= GL_PIXEL_BUFFER_BARRIER_BIT;
			}
			if ( p_barriers & toUnderlying( E_MEMORY_BARRIER::TEXTURE_UPDATE ) )
			{
				result |= GL_TEXTURE_UPDATE_BARRIER_BIT;
			}
			if ( p_barriers & toUnderlying( E_MEMORY_BARRIER::BUFFER_UPDATE ) )
			{
				result |= GL_BUFFER_UPDATE_BARRIER_BIT;
			}
			if ( p_barriers & toUnderlying( E_MEMORY_BARRIER::FRAMEBUFFER ) )
			{
				result |= GL_FRAMEBUFFER_BARRIER_BIT;
			}
			if ( p_barriers & toUnderlying( E_MEMORY_BARRIER::TRANSFORM_FEEDBACK ) )
			{
				result |= GL_TRANSFORM_FEEDBACK_BARRIER_BIT;
			}
			if ( p_barriers & toUnderlying( E_MEMORY_BARRIER::ATOMIC_COUNTER ) )
			{
				result |= GL_ATOMIC_COUNTER_BARRIER_BIT;
			}
			if ( p_barriers & toUnderlying( E_MEMORY_BARRIER::SHADER_STORAGE ) )
			{
				result |= GL_SHADER_STORAGE_BARRIER_BIT;
			}
			if ( p_barriers & toUnderlying( E_MEMORY_BARRIER::QUERY ) )
			{
				result |= GL_QUERY_BUFFER_BARRIER_BIT;
			}
			if ( p_barriers & toUnderlying( E_MEMORY_BARRIER::CLIENT_MAPPED_BUFFER ) )
			{
				result |= GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT;
			}

			return result;
		}

		static constexpr GLenum _toGL( const uint32_t p_primitive ) noexcept
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

		static constexpr GLenum _toGLShaderTarget( const Desc::E_BUFFER_USAGE p_usage ) noexcept
		{
			using namespace Desc;

			if ( Util::Enum::hasBits( p_usage, E_BUFFER_USAGE::UNIFORM ) )
			{
				return GL_UNIFORM_BUFFER;
			}
			if ( Util::Enum::hasBits( p_usage, E_BUFFER_USAGE::STORAGE ) )
			{
				return GL_SHADER_STORAGE_BUFFER;
			}

			assert( false );
			return GL_INVALID_INDEX;
		}

		static GLsizei _drawCapacity(
			const Backend::GL::Buffer & p_indirectBuffer,
			const uint32_t				p_commandOffset,
			const uint32_t				p_commandStride
		) noexcept
		{
			assert( p_commandStride > 0 );

			const GLsizei bufferSize = p_indirectBuffer.size();
			if ( p_commandOffset >= static_cast<uint32_t>( bufferSize ) )
			{
				return 0;
			}

			return static_cast<GLsizei>(
				( static_cast<uint32_t>( bufferSize ) - p_commandOffset ) / p_commandStride
			);
		}

		static void _dumpGLError() noexcept
		{
#ifdef _DEBUG
			Backend::GL::Debug::dumpGLError();
#endif
		}

		/**
		 * @brief Reference to the gl backend to access ressources.
		 */
		const Backend::OpenGL & _backend;
	};
} // namespace VTX::Renderer::Context::Executor

#endif
