#ifndef __VTX_GL_VERETX_ARRAY__
#define __VTX_GL_VERETX_ARRAY__

#ifdef _MSC_VER
#pragma once
#endif

#include "buffer.hpp"
#include "define.hpp"
#include "generic/base_opengl.hpp"
#include "vtx_app.hpp"

#define VTX_USE_OPENGL_MULTI_DRAW 1

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

		inline void create() { _gl->glCreateVertexArrays( 1, &_id ); }

		inline GLuint getId() const { return _id; }

		inline void bind() const { _gl->glBindVertexArray( _id ); }
		inline void unbind() const { _gl->glBindVertexArray( 0 ); }

		inline void bindElementBuffer( const Buffer & p_elementBuffer ) const
		{
			_gl->glVertexArrayElementBuffer( _id, p_elementBuffer.getId() );
		}

		inline void enableAttribute( const GLuint p_bindingIndex ) const
		{
			_gl->glEnableVertexArrayAttrib( _id, p_bindingIndex );
		}

		inline void setVertexBuffer( const GLuint	p_bindingIndex,
									 const Buffer & p_vertexBuffer,
									 const GLsizei	p_stride,
									 const GLintptr p_offset = 0 ) const
		{
			_gl->glVertexArrayVertexBuffer( _id, p_bindingIndex, p_vertexBuffer.getId(), p_offset, p_stride );
		}

		inline void setAttributeFormat( const GLuint	p_attributeIndex,
										const GLint		p_size,
										const Type		p_type,
										const GLuint	p_relativeOffset = 0,
										const GLboolean p_normalized	 = GL_FALSE ) const
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

		inline void setAttributeBinding( const GLuint p_attributeIndex, const GLuint p_bindingIndex ) const
		{
			_gl->glVertexArrayAttribBinding( _id, p_attributeIndex, p_bindingIndex );
		}

		inline void drawArray( const DrawMode p_mode, const GLint p_first, const GLsizei p_count ) const
		{
			bind();
			_gl->glDrawArrays( GLenum( p_mode ), p_first, p_count );
			unbind();
			VTX_STAT().drawCalls++;
		}

		inline void multiDrawArray( const DrawMode		  p_mode,
									const GLint * const	  p_first,
									const GLsizei * const p_count,
									const GLsizei		  p_primcount ) const
		{
			bind();
#if VTX_USE_OPENGL_MULTI_DRAW
			_gl->glMultiDrawArrays( GLenum( p_mode ), p_first, p_count, p_primcount );
#else
			for ( uint i = 0; i < p_primcount; i++ )
			{
				if ( p_count[ i ] > 0 )
				{
					glDrawArrays( GLenum( p_mode ), p_first[ i ], p_count[ i ] );
				}
			}
#endif
			unbind();
			VTX_STAT().drawCalls++;
		}

		inline void drawElement( const DrawMode		  p_mode,
								 const GLsizei		  p_count,
								 const Type			  p_type,
								 const GLvoid * const p_offset = 0 ) const
		{
			bind();
			_gl->glDrawElements( GLenum( p_mode ), p_count, GLenum( p_type ), p_offset );
			unbind();
			VTX_STAT().drawCalls++;
		}

		inline void multiDrawElement( const DrawMode			   p_mode,
									  const GLsizei * const		   p_count,
									  const Type				   p_type,
									  const GLvoid * const * const p_offset,
									  const GLsizei				   p_primcount ) const
		{
			bind();
#if VTX_USE_OPENGL_MULTI_DRAW
			_gl->glMultiDrawElements( GLenum( p_mode ), p_count, GLenum( p_type ), p_offset, p_primcount );
#else
			for ( uint i = 0; i < p_primcount; i++ )
			{
				if ( p_count[ i ] > 0 )
				{
					glDrawElements( GLenum( p_mode ), p_count[ i ], GLenum( p_type ), p_offset[ i ] );
				}
			}
#endif
			unbind();
			VTX_STAT().drawCalls++;
		}

	  private:
		GLuint _id = GL_INVALID_INDEX;
	};
} // namespace VTX::Renderer::GL

#endif // __VTX_GL_VERETX_ARRAY__
