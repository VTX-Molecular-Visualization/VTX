#ifndef __VTX_GL_VERETX_ARRAY__
#define __VTX_GL_VERETX_ARRAY__

#ifdef _MSC_VER
#pragma once
#endif

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

		VertexArray( OpenGLFunctions * const p_gl ) : BaseOpenGL( p_gl ) {}
		~VertexArray() { _gl->glDeleteVertexArrays( 1, &_id ); }

		void create() { _gl->glCreateVertexArrays( 1, &_id ); }

		GLuint getId() const { return _id; }

		void bind() const { _gl->glBindVertexArray( _id ); }
		void unbind() const { _gl->glBindVertexArray( 0 ); }

		void bindElementBuffer( const GLuint p_elementBuffer ) const
		{
			_gl->glVertexArrayElementBuffer( _id, p_elementBuffer );
		}

		void enableAttribute( const GLuint p_bindingIndex ) const
		{
			_gl->glEnableVertexArrayAttrib( _id, p_bindingIndex );
		}

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

		void drawArray();
		void drawElement();

	  private:
		GLuint _id = GL_INVALID_INDEX;
	};
} // namespace VTX::Renderer::GL

#endif // __VTX_GL_VERETX_ARRAY__
