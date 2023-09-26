#ifndef __VTX_RENDERER_CONTEXT_GL_VERTEX_ARRAY__
#define __VTX_RENDERER_CONTEXT_GL_VERTEX_ARRAY__

#include <glad/glad.h>
#include <type_traits>

namespace VTX::Renderer::Context::GL::VertexArray
{
	inline void create( GLuint * const p_handle )
	{
		glCreateVertexArrays( 1, p_handle );
		assert( glIsVertexArray( *p_handle ) );
	}

	inline void destroy( GLuint * const p_handle )
	{
		assert( glIsVertexArray( *p_handle ) );
		glDeleteVertexArrays( 1, p_handle );
	}

	inline void bind( const GLuint p_handle )
	{
		assert( glIsVertexArray( p_handle ) );
		glBindVertexArray( p_handle );
	}

	inline void unbind() { glBindVertexArray( 0 ); }

	inline void bindElementBuffer( const GLuint p_handle, const GLuint p_elementBuffer )
	{
		assert( glIsVertexArray( p_handle ) );
		glVertexArrayElementBuffer( p_handle, p_elementBuffer );
	}

	inline void unbindElementBuffer()
	{
		// 2023-07-25: Fail on Intel HD Graphics 520.
		// glVertexArrayElementBuffer( _id, 0 );
	}

	inline void enableAttribute( const GLuint p_handle, const GLuint p_bindingIndex )
	{
		assert( glIsVertexArray( p_handle ) );
		glEnableVertexArrayAttrib( p_handle, p_bindingIndex );
	}

	template<typename T>
	inline void setVertexBuffer( const GLuint	p_handle,
								 const GLuint	p_bindingIndex,
								 const GLuint	p_vertexBuffer,
								 const GLsizei	p_stride,
								 const GLintptr p_offset = 0 )
	{
		assert( glIsVertexArray( p_handle ) );
		glVertexArrayVertexBuffer( p_handle, p_bindingIndex, p_vertexBuffer, p_offset, p_stride );
	}

	template<typename T>
	inline void setAttributeFormat( const GLuint	p_handle,
									const GLuint	p_attributeIndex,
									const GLint		p_size,
									const GLuint	p_relativeOffset = 0,
									const GLboolean p_normalized	 = GL_FALSE )
	{
		assert( glIsVertexArray( p_handle ) );
		assert( false );
	}

	template<>
	inline void VertexArray::setAttributeFormat<float>( const GLuint	p_handle,
														const GLuint	p_attributeIndex,
														const GLint		p_size,
														const GLuint	p_relativeOffset,
														const GLboolean p_normalized )
	{
		assert( glIsVertexArray( p_handle ) );
		glVertexArrayAttribFormat( p_handle, p_attributeIndex, p_size, GL_FLOAT, p_normalized, p_relativeOffset );
	}

	template<>
	inline void VertexArray::setAttributeFormat<double>( const GLuint	 p_handle,
														 const GLuint	 p_attributeIndex,
														 const GLint	 p_size,
														 const GLuint	 p_relativeOffset,
														 const GLboolean p_normalized )
	{
		assert( glIsVertexArray( p_handle ) );
		glVertexArrayAttribLFormat( p_handle, p_attributeIndex, p_size, GL_DOUBLE, p_relativeOffset );
	}

	template<>
	inline void VertexArray::setAttributeFormat<uint>( const GLuint	   p_handle,
													   const GLuint	   p_attributeIndex,
													   const GLint	   p_size,
													   const GLuint	   p_relativeOffset,
													   const GLboolean p_normalized )
	{
		assert( glIsVertexArray( p_handle ) );
		glVertexArrayAttribIFormat( p_handle, p_attributeIndex, p_size, GL_UNSIGNED_INT, p_relativeOffset );
	}

	template<>
	inline void VertexArray::setAttributeFormat<int>( const GLuint	  p_handle,
													  const GLuint	  p_attributeIndex,
													  const GLint	  p_size,
													  const GLuint	  p_relativeOffset,
													  const GLboolean p_normalized )
	{
		assert( glIsVertexArray( p_handle ) );
		glVertexArrayAttribIFormat( p_handle, p_attributeIndex, p_size, GL_INT, p_relativeOffset );
	}

	inline void setAttributeBinding( const GLuint p_handle, const GLuint p_attributeIndex, const GLuint p_bindingIndex )
	{
		assert( glIsVertexArray( p_handle ) );
		glVertexArrayAttribBinding( p_handle, p_attributeIndex, p_bindingIndex );
	}

	inline void drawArray( const GLenum p_mode, const GLint p_first, const GLsizei p_count )
	{
		glDrawArrays( p_mode, p_first, p_count );
	}

	inline void multiDrawArray( const GLenum		  p_mode,
								const GLint * const	  p_first,
								const GLsizei * const p_count,
								const GLsizei		  p_primcount )
	{
		glMultiDrawArrays( p_mode, p_first, p_count, p_primcount );
	}

	inline void drawElement( const GLenum		  p_mode,
							 const GLsizei		  p_count,
							 const GLenum		  p_type,
							 const GLvoid * const p_offset = 0 )
	{
		glDrawElements( p_mode, p_count, p_type, p_offset );
	}

	inline void multiDrawElement( const GLenum				   p_mode,
								  const GLsizei * const		   p_count,
								  const GLenum				   p_type,
								  const GLvoid * const * const p_offset,
								  const GLsizei				   p_primcount )
	{
		glMultiDrawElements( p_mode, p_count, p_type, p_offset, p_primcount );
	}

} // namespace VTX::Renderer::Context::GL::VertexArray

#endif // __VTX_GL_VERETX_ARRAY__
