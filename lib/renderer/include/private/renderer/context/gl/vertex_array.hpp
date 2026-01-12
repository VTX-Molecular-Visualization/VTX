#ifndef __VTX_RENDERER_CONTEXT_GL_VERTEX_ARRAY__
#define __VTX_RENDERER_CONTEXT_GL_VERTEX_ARRAY__

#include "include_opengl.hpp"
#include <type_traits>

namespace VTX::Renderer::Context::GL
{
	class VertexArray
	{
	  public:
		VertexArray()
		{
			assert( _id == GL_INVALID_INDEX );

			glCreateVertexArrays( 1, &_id );
		}

		~VertexArray() noexcept
		{
			if ( _id != GL_INVALID_INDEX )
			{
				glDeleteVertexArrays( 1, &_id );
				_id = GL_INVALID_INDEX;
			}
		}

		// int drawCalls = 0;

		inline GLuint getId() const noexcept { return _id; }

		inline void bind() const noexcept
		{
			assert( glIsVertexArray( _id ) );

			glBindVertexArray( _id );
		}

		inline void unbind() const noexcept { glBindVertexArray( 0 ); }

		inline void bindElementBuffer( const Buffer & p_elementBuffer ) const noexcept
		{
			assert( glIsVertexArray( _id ) );

			glVertexArrayElementBuffer( _id, p_elementBuffer.getId() );
		}

		inline void unbindElementBuffer() const noexcept
		{
			assert( glIsVertexArray( _id ) );

			// 2023-07-25: Fail on Intel HD Graphics 520.
			// glVertexArrayElementBuffer( _id, 0 );
		}

		inline void enableAttribute( const GLuint p_bindingIndex ) const noexcept
		{
			assert( glIsVertexArray( _id ) );

			glEnableVertexArrayAttrib( _id, p_bindingIndex );
		}

		inline void setVertexBuffer(
			const GLuint   p_bindingIndex,
			const Buffer & p_vertexBuffer,
			const GLsizei  p_stride,
			const GLintptr p_offset = 0
		) const noexcept
		{
			assert( glIsVertexArray( _id ) );

			glVertexArrayVertexBuffer( _id, p_bindingIndex, p_vertexBuffer.getId(), p_offset, p_stride );
		}

		inline void setAttributeFormat(
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

		inline void setAttributeBinding( const GLuint p_attributeIndex, const GLuint p_bindingIndex ) const noexcept
		{
			assert( glIsVertexArray( _id ) );

			glVertexArrayAttribBinding( _id, p_attributeIndex, p_bindingIndex );
		}

		inline void drawArray( const GLenum p_mode, const GLint p_first, const GLsizei p_count ) const noexcept
		{
			glDrawArrays( p_mode, p_first, p_count );
		}

		inline void multiDrawArray(
			const GLenum		  p_mode,
			const GLint * const	  p_first,
			const GLsizei * const p_count,
			const GLsizei		  p_primcount
		) const noexcept
		{
			glMultiDrawArrays( p_mode, p_first, p_count, p_primcount );
		}

		inline void drawElement(
			const GLenum		 p_mode,
			const GLsizei		 p_count,
			const GLenum		 p_type,
			const GLvoid * const p_offset = 0
		) const noexcept
		{
			glDrawElements( p_mode, p_count, p_type, p_offset );
		}

		inline void multiDrawElement(
			const GLenum				 p_mode,
			const GLsizei * const		 p_count,
			const GLenum				 p_type,
			const GLvoid * const * const p_offset,
			const GLsizei				 p_primcount
		) const noexcept
		{
			glMultiDrawElements( p_mode, p_count, p_type, p_offset, p_primcount );
		}

	  private:
		GLuint _id = GL_INVALID_INDEX;
	};
} // namespace VTX::Renderer::Context::GL

#endif // __VTX_GL_VERETX_ARRAY__
