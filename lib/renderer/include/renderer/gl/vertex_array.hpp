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
			bind();
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, p_elementBuffer.getId() );
			unbind();
#endif
		}

		inline void enableAttribute( const GLuint p_bindingIndex ) const
		{
			assert( glIsVertexArray( _id ) );

#if ( VTX_OPENGL_VERSION == 450 )
			glEnableVertexArrayAttrib( _id, p_bindingIndex );
#else
			bind();
			glEnableVertexAttribArray( p_bindingIndex );
			unbind();
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
				bind();
				glVertexAttribPointer(
					p_bindingIndex, p_stride, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void *>( p_offset ) );
				unbind();
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

		template<>
		inline void setAttributeFormat<float>( const GLuint	   p_attributeIndex,
											   const GLint	   p_size,
											   const GLuint	   p_relativeOffset,
											   const GLboolean p_normalized ) const
		{
			assert( glIsVertexArray( _id ) );
			glVertexArrayAttribFormat( _id, p_attributeIndex, p_size, GL_FLOAT, p_normalized, p_relativeOffset );
		}

		template<>
		inline void setAttributeFormat<double>( const GLuint	p_attributeIndex,
												const GLint		p_size,
												const GLuint	p_relativeOffset,
												const GLboolean p_normalized ) const
		{
			assert( glIsVertexArray( _id ) );
			glVertexArrayAttribLFormat( _id, p_attributeIndex, p_size, GL_DOUBLE, p_relativeOffset );
		}

		template<>
		inline void setAttributeFormat<uint>( const GLuint	  p_attributeIndex,
											  const GLint	  p_size,
											  const GLuint	  p_relativeOffset,
											  const GLboolean p_normalized ) const
		{
			assert( glIsVertexArray( _id ) );
			glVertexArrayAttribIFormat( _id, p_attributeIndex, p_size, GL_UNSIGNED_INT, p_relativeOffset );
		}

		template<>
		inline void setAttributeFormat<int>( const GLuint	 p_attributeIndex,
											 const GLint	 p_size,
											 const GLuint	 p_relativeOffset,
											 const GLboolean p_normalized ) const
		{
			assert( glIsVertexArray( _id ) );
			glVertexArrayAttribIFormat( _id, p_attributeIndex, p_size, GL_INT, p_relativeOffset );
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
			bind();
			glDrawArrays( p_mode, p_first, p_count );
			// drawCalls++;
			unbind();
		}

		inline void multiDrawArray( const GLenum		  p_mode,
									const GLint * const	  p_first,
									const GLsizei * const p_count,
									const GLsizei		  p_primcount )
		{
			bind();
			glMultiDrawArrays( p_mode, p_first, p_count, p_primcount );
			// drawCalls++;
			unbind();
		}

		inline void drawElement( const GLenum		  p_mode,
								 const GLsizei		  p_count,
								 const GLenum		  p_type,
								 const GLvoid * const p_offset = 0 )
		{
			bind();
			glDrawElements( p_mode, p_count, p_type, p_offset );
			// drawCalls++;
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
			// drawCalls++;
			unbind();
		}

	  private:
		GLuint _id = GL_INVALID_INDEX;
	};
} // namespace VTX::Renderer::GL

#endif // __VTX_GL_VERETX_ARRAY__
