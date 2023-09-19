#ifndef __VTX_GL_VERETX_ARRAY__
#define __VTX_GL_VERETX_ARRAY__

#include "renderer/gl/include_opengl.hpp"
#include <type_traits>

namespace VTX::Renderer::GL
{
	class VertexArray
	{
	  public:
		VertexArray()
		{
			assert( _id == GL_INVALID_INDEX );

#if ( VTX_OPENGL_VERSION == 450 )
			glCreateVertexArrays( 1, &_id );
#else
			glGenVertexArrays( 1, &_id );
#endif
		}

		~VertexArray()
		{
			if ( _id != GL_INVALID_INDEX )
			{
				glDeleteVertexArrays( 1, &_id );
				_id = GL_INVALID_INDEX;
			}
		}

		// int drawCalls = 0;

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

#if ( VTX_OPENGL_VERSION == 450 )
			glVertexArrayElementBuffer( _id, p_elementBuffer.getId() );
#else
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, p_elementBuffer.getId() );
#endif
		}

		inline void unbindElementBuffer() const
		{
			assert( glIsVertexArray( _id ) );

#if ( VTX_OPENGL_VERSION == 450 )
			// 2023-07-25: Fail on Intel HD Graphics 520.
			// glVertexArrayElementBuffer( _id, 0 );
#else
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
#endif
		}

		inline void enableAttribute( const GLuint p_bindingIndex ) const
		{
			assert( glIsVertexArray( _id ) );

#if ( VTX_OPENGL_VERSION == 450 )
			glEnableVertexArrayAttrib( _id, p_bindingIndex );
#else
			glEnableVertexAttribArray( p_bindingIndex );
#endif
		}

		template<typename T>
		inline void setVertexBuffer( const GLuint	p_bindingIndex,
									 const Buffer & p_vertexBuffer,
									 const GLsizei	p_stride,
									 const GLintptr p_offset = 0 ) const
		{
			assert( glIsVertexArray( _id ) );

#if ( VTX_OPENGL_VERSION == 450 )
			glVertexArrayVertexBuffer( _id, p_bindingIndex, p_vertexBuffer.getId(), p_offset, p_stride );
#else
			if ( std::is_same<T, float>::value )
			{
				glVertexAttribPointer(
					p_bindingIndex, p_stride, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void *>( p_offset ) );
			}
			else
			{
				assert( false );
				// TODO: handle more types.
			}
#endif
		}

		template<typename T>
		inline void setAttributeFormat( const GLuint	p_attributeIndex,
										const GLint		p_size,
										const GLuint	p_relativeOffset = 0,
										const GLboolean p_normalized	 = GL_FALSE ) const
		{
			assert( glIsVertexArray( _id ) );

			assert( false );
		}

		inline void setAttributeBinding( const GLuint p_attributeIndex, const GLuint p_bindingIndex ) const
		{
			assert( glIsVertexArray( _id ) );

#if ( VTX_OPENGL_VERSION == 450 )
			glVertexArrayAttribBinding( _id, p_attributeIndex, p_bindingIndex );
#endif
		}

		inline void drawArray( const GLenum p_mode, const GLint p_first, const GLsizei p_count )
		{
			glDrawArrays( p_mode, p_first, p_count );
			// drawCalls++;
		}

		inline void multiDrawArray( const GLenum		  p_mode,
									const GLint * const	  p_first,
									const GLsizei * const p_count,
									const GLsizei		  p_primcount )
		{
			glMultiDrawArrays( p_mode, p_first, p_count, p_primcount );
			// drawCalls++;
		}

		inline void drawElement( const GLenum		  p_mode,
								 const GLsizei		  p_count,
								 const GLenum		  p_type,
								 const GLvoid * const p_offset = 0 )
		{
			glDrawElements( p_mode, p_count, p_type, p_offset );
			// drawCalls++;
		}

		inline void multiDrawElement( const GLenum				   p_mode,
									  const GLsizei * const		   p_count,
									  const GLenum				   p_type,
									  const GLvoid * const * const p_offset,
									  const GLsizei				   p_primcount )
		{
			glMultiDrawElements( p_mode, p_count, p_type, p_offset, p_primcount );
			// drawCalls++;
		}

	  private:
		GLuint _id = GL_INVALID_INDEX;
	};

	template<>
	inline void VertexArray::setAttributeFormat<float>( const GLuint	p_attributeIndex,
														const GLint		p_size,
														const GLuint	p_relativeOffset,
														const GLboolean p_normalized ) const
	{
		assert( glIsVertexArray( _id ) );

		glVertexArrayAttribFormat( _id, p_attributeIndex, p_size, GL_FLOAT, p_normalized, p_relativeOffset );
	}

	template<>
	inline void VertexArray::setAttributeFormat<double>( const GLuint	 p_attributeIndex,
														 const GLint	 p_size,
														 const GLuint	 p_relativeOffset,
														 const GLboolean p_normalized ) const
	{
		assert( glIsVertexArray( _id ) );

		glVertexArrayAttribLFormat( _id, p_attributeIndex, p_size, GL_DOUBLE, p_relativeOffset );
	}

	template<>
	inline void VertexArray::setAttributeFormat<uint>( const GLuint	   p_attributeIndex,
													   const GLint	   p_size,
													   const GLuint	   p_relativeOffset,
													   const GLboolean p_normalized ) const
	{
		assert( glIsVertexArray( _id ) );

		glVertexArrayAttribIFormat( _id, p_attributeIndex, p_size, GL_UNSIGNED_INT, p_relativeOffset );
	}

	template<>
	inline void VertexArray::setAttributeFormat<int>( const GLuint	  p_attributeIndex,
													  const GLint	  p_size,
													  const GLuint	  p_relativeOffset,
													  const GLboolean p_normalized ) const
	{
		assert( glIsVertexArray( _id ) );

		glVertexArrayAttribIFormat( _id, p_attributeIndex, p_size, GL_INT, p_relativeOffset );
	}
} // namespace VTX::Renderer::GL

#endif // __VTX_GL_VERETX_ARRAY__
