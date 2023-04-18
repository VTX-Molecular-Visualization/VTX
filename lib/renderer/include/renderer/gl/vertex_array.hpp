#ifndef __VTX_GL_VERETX_ARRAY__
#define __VTX_GL_VERETX_ARRAY__

#define VTX_USE_OPENGL_MULTI_DRAW 1

namespace VTX::Renderer::GL
{
	// TODO: methods such as 'enableAttrib', 'setVertexBuffer', etc. might be merged...
	class VertexArray
	{
	  public:
		VertexArray() = default;
		~VertexArray() { destroy(); }

		inline void create() { glCreateVertexArrays( 1, &_id ); }
		inline void destroy() { glDeleteVertexArrays( 1, &_id ); }

		inline GLuint getId() const { return _id; }

		inline void bind() const { glBindVertexArray( _id ); }
		inline void unbind() const { glBindVertexArray( 0 ); }

		inline void bindElementBuffer( const Buffer & p_elementBuffer ) const
		{
			glVertexArrayElementBuffer( _id, p_elementBuffer.getId() );
		}

		inline void enableAttribute( const GLuint p_bindingIndex ) const
		{
			glEnableVertexArrayAttrib( _id, p_bindingIndex );
		}

		inline void setVertexBuffer( const GLuint	p_bindingIndex,
									 const Buffer & p_vertexBuffer,
									 const GLsizei	p_stride,
									 const GLintptr p_offset = 0 ) const
		{
			glVertexArrayVertexBuffer( _id, p_bindingIndex, p_vertexBuffer.getId(), p_offset, p_stride );
		}

		inline void setAttributeFormat( const GLuint	p_attributeIndex,
										const GLint		p_size,
										const GLenum	p_type,
										const GLuint	p_relativeOffset = 0,
										const GLboolean p_normalized	 = GL_FALSE ) const
		{
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
			glVertexArrayAttribBinding( _id, p_attributeIndex, p_bindingIndex );
		}

		inline void drawArray( const GLenum p_mode, const GLint p_first, const GLsizei p_count ) const
		{
			bind();
			glDrawArrays( p_mode, p_first, p_count );
			// VTX_STAT().drawCalls++;
			unbind();
		}

		inline void multiDrawArray( const GLenum		  p_mode,
									const GLint * const	  p_first,
									const GLsizei * const p_count,
									const GLsizei		  p_primcount ) const
		{
			bind();
#if VTX_USE_OPENGL_MULTI_DRAW
			glMultiDrawArrays( p_mode, p_first, p_count, p_primcount );
			// VTX_STAT().drawCalls++;
#else
			for ( uint i = 0; i < uint( p_primcount ); i++ )
			{
				if ( p_count[ i ] > 0 )
				{
					glDrawArrays(  p_mode ), p_first[ i ], p_count[ i ] );
					// VTX_STAT().drawCalls++;
				}
			}
#endif
			unbind();
		}

		inline void drawElement( const GLenum		  p_mode,
								 const GLsizei		  p_count,
								 const GLenum		  p_type,
								 const GLvoid * const p_offset = 0 ) const
		{
			bind();
			glDrawElements( p_mode, p_count, p_type, p_offset );
			// VTX_STAT().drawCalls++;
			unbind();
		}

		inline void multiDrawElement( const GLenum				   p_mode,
									  const GLsizei * const		   p_count,
									  const GLenum				   p_type,
									  const GLvoid * const * const p_offset,
									  const GLsizei				   p_primcount ) const
		{
			bind();
#if VTX_USE_OPENGL_MULTI_DRAW
			glMultiDrawElements( p_mode, p_count, p_type, p_offset, p_primcount );
			// VTX_STAT().drawCalls++;
#else
			for ( uint i = 0; i < uint( p_primcount ); i++ )
			{
				if ( p_count[ i ] > 0 )
				{
					glDrawElements( p_mode, p_count[ i ], p_type, p_offset[ i ] );
					// VTX_STAT().drawCalls++;
				}
			}
#endif
			unbind();
		}

	  private:
		GLuint _id = GL_INVALID_INDEX;
	};
} // namespace VTX::Renderer::GL

#endif // __VTX_GL_VERETX_ARRAY__
