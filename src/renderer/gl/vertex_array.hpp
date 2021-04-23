#ifndef __VTX_GL_VERETX_ARRAY__
#define __VTX_GL_VERETX_ARRAY__

#ifdef _MSC_VER
#pragma once
#endif

#include "buffer.hpp"
#include "define.hpp"
#include "generic/base_opengl.hpp"

namespace VTX::Renderer::GL
{
	// TODO: methods such as 'enableAttrib', 'setVertexBuffer', etc. might be merged...
	class VertexArray : public Generic::BaseOpenGL
	{
	  public:
		enum class Type : GLenum
		{
			BYTE		   = GL_BYTE,
			UNSIGNED_BYTE  = GL_UNSIGNED_BYTE,
			SHORT		   = GL_SHORT,
			UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
			INT			   = GL_INT,
			UNSIGNED_INT   = GL_UNSIGNED_INT,
			FLOAT		   = GL_FLOAT,
			DOUBLE		   = GL_DOUBLE
			/// TODO: complete if needed.
		};

		enum class DrawMode : GLenum
		{
			POINTS					 = GL_POINTS,
			LINE_STRIP				 = GL_LINE_STRIP,
			LINE_LOOP				 = GL_LINE_LOOP,
			LINES					 = GL_LINES,
			LINE_STRIP_ADJACENCY	 = GL_LINE_STRIP_ADJACENCY,
			LINES_ADJACENCY			 = GL_LINES_ADJACENCY,
			TRIANGLE_STRIP			 = GL_TRIANGLE_STRIP,
			TRIANGLE_FAN			 = GL_TRIANGLE_FAN,
			TRIANGLES				 = GL_TRIANGLES,
			TRIANGLE_STRIP_ADJACENCY = GL_TRIANGLE_STRIP_ADJACENCY,
			TRIANGLES_ADJACENCY		 = GL_TRIANGLES_ADJACENCY,
			PATCHES					 = GL_PATCHES
		};

		VertexArray( OpenGLFunctions * const p_gl ) : BaseOpenGL( p_gl ) {}
		~VertexArray() { _gl->glDeleteVertexArrays( 1, &_id ); }

		void create() { _gl->glCreateVertexArrays( 1, &_id ); }

		GLuint getId() const { return _id; }

		void bind() const { _gl->glBindVertexArray( _id ); }
		void unbind() const { _gl->glBindVertexArray( 0 ); }

		void bindElementBuffer( const Buffer & p_elementBuffer ) const
		{
			_gl->glVertexArrayElementBuffer( _id, p_elementBuffer.getId() );
		}
		/// TODO: remove vbo GLuint version
		void bindElementBuffer( const GLuint p_elementBuffer ) const
		{
			_gl->glVertexArrayElementBuffer( _id, p_elementBuffer );
		}

		void enableAttribute( const GLuint p_bindingIndex ) const
		{
			_gl->glEnableVertexArrayAttrib( _id, p_bindingIndex );
		}

		void setVertexBuffer( const GLuint	 p_bindingIndex,
							  const Buffer & p_vertexBuffer,
							  const GLsizei	 p_stride,
							  const GLintptr p_offset = 0 ) const
		{
			_gl->glVertexArrayVertexBuffer( _id, p_bindingIndex, p_vertexBuffer.getId(), p_offset, p_stride );
		}
		/// TODO: remove vbo GLuint version
		void setVertexBuffer( const GLuint	 p_bindingIndex,
							  const GLuint	 p_vertexBuffer,
							  const GLsizei	 p_stride,
							  const GLintptr p_offset = 0 ) const
		{
			_gl->glVertexArrayVertexBuffer( _id, p_bindingIndex, p_vertexBuffer, p_offset, p_stride );
		}
		void setAttributeFormat( const GLuint	 p_attributeIndex,
								 const GLint	 p_size,
								 const Type		 p_type,
								 const GLuint	 p_relativeOffset = 0,
								 const GLboolean p_normalized	  = GL_FALSE ) const
		{
			// TODO: split ? test !
			if ( p_type == Type::FLOAT )
			{
				_gl->glVertexArrayAttribFormat(
					_id, p_attributeIndex, p_size, GLenum( p_type ), p_normalized, p_relativeOffset );
			}
			else if ( p_type == Type::DOUBLE )
			{
				_gl->glVertexArrayAttribLFormat( _id, p_attributeIndex, p_size, GLenum( p_type ), p_relativeOffset );
			}
			else // integers
			{
				_gl->glVertexArrayAttribIFormat( _id, p_attributeIndex, p_size, GLenum( p_type ), p_relativeOffset );
			}
		}
		void setAttributeBinding( const GLuint p_attributeIndex, const GLuint p_bindingIndex ) const
		{
			_gl->glVertexArrayAttribBinding( _id, p_attributeIndex, p_bindingIndex );
		}

		void drawArray( const DrawMode p_mode, const GLint p_first, const GLsizei p_count ) const
		{
			bind();
			_gl->glDrawArrays( GLenum( p_mode ), p_first, p_count );
			unbind();
		}
		void drawElement( const DrawMode p_mode,
						  const GLsizei	 p_count,
						  const Type	 p_type,
						  const GLvoid * p_offset = 0 ) const
		{
			bind();
			_gl->glDrawElements( GLenum( p_mode ), p_count, GLenum( p_type ), p_offset );
			unbind();
		}

	  private:
		GLuint _id = GL_INVALID_INDEX;
	};
} // namespace VTX::Renderer::GL

#endif // __VTX_GL_VERETX_ARRAY__
