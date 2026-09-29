#ifndef __VTX_RENDERER_CONTEXT_GL_VERTEX_ARRAY__
#define __VTX_RENDERER_CONTEXT_GL_VERTEX_ARRAY__

#include "include_opengl.hpp"
#include <type_traits>
#include <utility>

namespace VTX::Renderer::Context::Backend::GL
{
	class VertexArray
	{
	  public:
		VertexArray()
		{
			assert( _id == GL_INVALID_INDEX );

			glCreateVertexArrays( 1, &_id );
		}

		VertexArray( const VertexArray & )			   = delete;
		VertexArray & operator=( const VertexArray & ) = delete;

		VertexArray( VertexArray && p_other ) noexcept : _id( std::exchange( p_other._id, GL_INVALID_INDEX ) ) {}

		VertexArray & operator=( VertexArray && p_other ) noexcept
		{
			if ( this != &p_other )
			{
				destroy();
				_id = std::exchange( p_other._id, GL_INVALID_INDEX );
			}

			return *this;
		}

		~VertexArray() noexcept { destroy(); }

		void destroy() noexcept
		{
			if ( _id != GL_INVALID_INDEX )
			{
				glDeleteVertexArrays( 1, &_id );
				_id = GL_INVALID_INDEX;
			}
		}

		// int drawCalls = 0;

		GLuint getId() const noexcept { return _id; }

		void bind() const noexcept
		{
			assert( glIsVertexArray( _id ) );

			glBindVertexArray( _id );
		}

		static void unbind() noexcept { glBindVertexArray( 0 ); }

		void bindElementBuffer( const Buffer & p_elementBuffer ) const noexcept
		{
			const GLuint id = p_elementBuffer.getId();

			assert( id != 0 );
			assert( glIsBuffer( id ) );
			assert( glIsVertexArray( _id ) );

			glVertexArrayElementBuffer( _id, p_elementBuffer.getId() );
		}

		void unbindElementBuffer() const noexcept
		{
			assert( glIsVertexArray( _id ) );

			// 2023-07-25: Fail on Intel HD Graphics 520.
			// glVertexArrayElementBuffer( _id, 0 );
		}

		void enableAttribute( const GLuint p_bindingIndex ) const noexcept
		{
			assert( glIsVertexArray( _id ) );

			glEnableVertexArrayAttrib( _id, p_bindingIndex );
		}

		void setVertexBuffer(
			const GLuint   p_bindingIndex,
			const Buffer & p_vertexBuffer,
			const GLintptr p_offset,
			const GLsizei  p_stride
		) const noexcept
		{
			const GLuint bid = p_vertexBuffer.getId();

			assert( glIsVertexArray( _id ) );
			assert( bid != 0 );
			assert( glIsBuffer( bid ) );
			assert( p_stride > 0 );
			assert( glIsVertexArray( _id ) );

			glVertexArrayVertexBuffer( _id, p_bindingIndex, bid, p_offset, p_stride );
		}

		void setAttributeFormat(
			const GLuint	p_attributeIndex,
			const GLint		p_size,
			const GLenum	p_type,
			const GLuint	p_relativeOffset = 0,
			const GLboolean p_normalized	 = GL_FALSE
		) const noexcept
		{
			assert( glIsVertexArray( _id ) );

			// Check type to call glVertexAttribFormat, glVertexAttribIFormat, glVertexAttribLFormat.

			switch ( p_type )
			{
			case GL_FLOAT:
				glVertexArrayAttribFormat( _id, p_attributeIndex, p_size, p_type, p_normalized, p_relativeOffset );
				break;
			case GL_BYTE:
			case GL_SHORT:
			case GL_INT:
			case GL_UNSIGNED_BYTE:
			case GL_UNSIGNED_SHORT:
			case GL_UNSIGNED_INT:
				glVertexArrayAttribIFormat( _id, p_attributeIndex, p_size, p_type, p_relativeOffset );
				break;

			case GL_DOUBLE:
				glVertexArrayAttribLFormat( _id, p_attributeIndex, p_size, p_type, p_relativeOffset );
				break;
			default: assert( false ); break;
			}
		}

		void setAttributeBinding( const GLuint p_attributeIndex, const GLuint p_bindingIndex ) const noexcept
		{
			assert( glIsVertexArray( _id ) );

			glVertexArrayAttribBinding( _id, p_attributeIndex, p_bindingIndex );
		}

		static void drawArrays( const GLenum p_mode, const GLint p_first, const GLsizei p_count ) noexcept
		{ glDrawArrays( p_mode, p_first, p_count ); }

		static void multiDrawArrays(
			const GLenum		  p_mode,
			const GLint * const	  p_first,
			const GLsizei * const p_count,
			const GLsizei		  p_primcount
		) noexcept
		{ glMultiDrawArrays( p_mode, p_first, p_count, p_primcount ); }

		static void multiDrawArraysIndirect(
			const GLenum		 p_mode,
			const GLvoid * const p_indirect,
			const GLsizei		 p_drawCount,
			const GLsizei		 p_stride
		) noexcept
		{ glMultiDrawArraysIndirect( p_mode, p_indirect, p_drawCount, p_stride ); }

		static void multiDrawArraysIndirectCount(
			const GLenum		 p_mode,
			const GLvoid * const p_indirect,
			const GLintptr		 p_drawCountOffset,
			const GLsizei		 p_drawCapacity,
			const GLsizei		 p_stride
		) noexcept
		{ glMultiDrawArraysIndirectCount( p_mode, p_indirect, p_drawCountOffset, p_drawCapacity, p_stride ); }

		static void drawElements(
			const GLenum		 p_mode,
			const GLsizei		 p_count,
			const GLenum		 p_type,
			const GLvoid * const p_offset = nullptr
		) noexcept
		{ glDrawElements( p_mode, p_count, p_type, p_offset ); }

		static void multiDrawElements(
			const GLenum				 p_mode,
			const GLsizei * const		 p_count,
			const GLenum				 p_type,
			const GLvoid * const * const p_offset,
			const GLsizei				 p_primcount
		) noexcept
		{ glMultiDrawElements( p_mode, p_count, p_type, p_offset, p_primcount ); }

		static void multiDrawElementsIndirect(
			const GLenum		 p_mode,
			const GLenum		 p_type,
			const GLvoid * const p_indirect,
			const GLsizei		 p_drawCount,
			const GLsizei		 p_stride
		) noexcept
		{ glMultiDrawElementsIndirect( p_mode, p_type, p_indirect, p_drawCount, p_stride ); }

		static void multiDrawElementsIndirectCount(
			const GLenum		 p_mode,
			const GLenum		 p_type,
			const GLvoid * const p_indirect,
			const GLintptr		 p_drawCountOffset,
			const GLsizei		 p_drawCapacity,
			const GLsizei		 p_stride
		) noexcept
		{ glMultiDrawElementsIndirectCount( p_mode, p_type, p_indirect, p_drawCountOffset, p_drawCapacity, p_stride ); }

		bool hasEbo() const noexcept
		{
			GLint ebo = 0;
			glGetVertexArrayiv( _id, GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebo );
			return ebo != 0;
		}

	  private:
		GLuint _id = GL_INVALID_INDEX;
	};
} // namespace VTX::Renderer::Context::Backend::GL

#endif // __VTX_GL_VERETX_ARRAY__
