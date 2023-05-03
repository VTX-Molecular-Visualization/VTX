#ifndef __VTX_GL_VERETX_ARRAY__
#define __VTX_GL_VERETX_ARRAY__

#include "renderer/gl/include_opengl.hpp"

namespace VTX::Renderer::GL
{
	class VertexArray
	{
	  public:
		VertexArray() = default;
		~VertexArray() { destroy(); }

		int drawCalls = 0;

		inline void create()
		{
			assert( _id == GL_INVALID_INDEX );

			glCreateVertexArrays( 1, &_id );
		}
		inline void destroy()
		{
			if ( _id != GL_INVALID_INDEX )
			{
				glDeleteVertexArrays( 1, &_id );
				_id = GL_INVALID_INDEX;
			}
		}

		inline GLuint getId() const { return _id; }

		inline void bind() const
		{
			assert( glIsVertexArray( _id ) );

			glBindVertexArray( _id );
		}

		inline void unbind() const { glBindVertexArray( 0 ); }

		inline void bindElementBuffer( const Buffer & p_elementBuffer ) const
		{
			assert( glIsVertexArray( _id ) );

			glVertexArrayElementBuffer( _id, p_elementBuffer.getId() );
		}

		inline void enableAttribute( const GLuint p_bindingIndex ) const
		{
			assert( glIsVertexArray( _id ) );

			glEnableVertexArrayAttrib( _id, p_bindingIndex );
		}

		inline void setVertexBuffer( const GLuint	p_bindingIndex,
									 const Buffer & p_vertexBuffer,
									 const GLsizei	p_stride,
									 const GLintptr p_offset = 0 ) const
		{
			assert( glIsVertexArray( _id ) );

			glVertexArrayVertexBuffer( _id, p_bindingIndex, p_vertexBuffer.getId(), p_offset, p_stride );
		}

		inline void setAttributeFormat( const GLuint	p_attributeIndex,
										const GLint		p_size,
										const GLenum	p_type,
										const GLuint	p_relativeOffset = 0,
										const GLboolean p_normalized	 = GL_FALSE ) const
		{
			assert( glIsVertexArray( _id ) );

			// TODO: split ? test !
			if ( p_type == GL_FLOAT )
			{
				glVertexArrayAttribFormat( _id, p_attributeIndex, p_size, p_type, p_normalized, p_relativeOffset );
			}
			else if ( p_type == GL_DOUBLE )
			{
				glVertexArrayAttribLFormat( _id, p_attributeIndex, p_size, p_type, p_relativeOffset );
			}
			else // integers
			{
				glVertexArrayAttribIFormat( _id, p_attributeIndex, p_size, p_type, p_relativeOffset );
			}
		}

		inline void setAttributeBinding( const GLuint p_attributeIndex, const GLuint p_bindingIndex ) const
		{
			assert( glIsVertexArray( _id ) );

			glVertexArrayAttribBinding( _id, p_attributeIndex, p_bindingIndex );
		}

		inline void drawArray( const GLenum p_mode, const GLint p_first, const GLsizei p_count )
		{
			bind();
			glDrawArrays( p_mode, p_first, p_count );
			drawCalls++;
			unbind();
		}

		inline void multiDrawArray( const GLenum		  p_mode,
									const GLint * const	  p_first,
									const GLsizei * const p_count,
									const GLsizei		  p_primcount )
		{
			bind();
			glMultiDrawArrays( p_mode, p_first, p_count, p_primcount );
			drawCalls++;
			unbind();
		}

		inline void drawElement( const GLenum		  p_mode,
								 const GLsizei		  p_count,
								 const GLenum		  p_type,
								 const GLvoid * const p_offset = 0 )
		{
			bind();
			glDrawElements( p_mode, p_count, p_type, p_offset );
			drawCalls++;
			unbind();
		}

		inline void multiDrawElement( const GLenum				   p_mode,
									  const GLsizei * const		   p_count,
									  const GLenum				   p_type,
									  const GLvoid * const * const p_offset,
									  const GLsizei				   p_primcount )
		{
			bind();
			glMultiDrawElements( p_mode, p_count, p_type, p_offset, p_primcount );
			drawCalls++;
			unbind();
		}

	  private:
		GLuint _id = GL_INVALID_INDEX;
	};
} // namespace VTX::Renderer::GL

#endif // __VTX_GL_VERETX_ARRAY__
