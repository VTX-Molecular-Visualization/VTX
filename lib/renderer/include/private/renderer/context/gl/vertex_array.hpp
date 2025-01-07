#ifndef __VTX_RENDERER_CONTEXT_GL_VERTEX_ARRAY__
#define __VTX_RENDERER_CONTEXT_GL_VERTEX_ARRAY__

#include <glad/glad.h>
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

		~VertexArray()
		{
			if ( _id != GL_INVALID_INDEX )
			{
				glDeleteVertexArrays( 1, &_id );
				_id = GL_INVALID_INDEX;
			}
		}

		// int drawCalls = 0;

		inline uint32_t getId() const { return _id; }

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

		inline void unbindElementBuffer() const
		{
			assert( glIsVertexArray( _id ) );

			// 2023-07-25: Fail on Intel HD Graphics 520.
			// glVertexArrayElementBuffer( _id, 0 );
		}

		inline void enableAttribute( const uint32_t p_bindingIndex ) const
		{
			assert( glIsVertexArray( _id ) );

			glEnableVertexArrayAttrib( _id, p_bindingIndex );
		}

		inline void setVertexBuffer(
			const uint32_t p_bindingIndex,
			const Buffer & p_vertexBuffer,
			const int32_t  p_stride,
			const int64_t  p_offset = 0
		) const
		{
			assert( glIsVertexArray( _id ) );

			glVertexArrayVertexBuffer( _id, p_bindingIndex, p_vertexBuffer.getId(), p_offset, p_stride );
		}

		template<typename T>
		inline void setAttributeFormat(
			const uint32_t p_attributeIndex,
			const int32_t  p_size,
			const uint32_t p_relativeOffset = 0,
			const uint8_t  p_normalized		= GL_FALSE
		) const
		{
			assert( glIsVertexArray( _id ) );

			assert( false );
		}

		inline void setAttributeFormat(
			const uint32_t p_attributeIndex,
			const int32_t  p_size,
			const uint32_t p_type,
			const uint32_t p_relativeOffset = 0,
			const uint8_t  p_normalized		= GL_FALSE
		) const
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

		inline void setAttributeBinding( const uint32_t p_attributeIndex, const uint32_t p_bindingIndex ) const
		{
			assert( glIsVertexArray( _id ) );

			glVertexArrayAttribBinding( _id, p_attributeIndex, p_bindingIndex );
		}

		inline void drawArray( const uint32_t p_mode, const int32_t p_first, const int32_t p_count )
		{
			glDrawArrays( p_mode, p_first, p_count );
		}

		inline void multiDrawArray(
			const uint32_t		  p_mode,
			const int32_t * const p_first,
			const int32_t * const p_count,
			const int32_t		  p_primcount
		)
		{
			glMultiDrawArrays( p_mode, p_first, p_count, p_primcount );
		}

		inline void drawElement(
			const uint32_t		 p_mode,
			const int32_t		 p_count,
			const uint32_t		 p_type,
			const GLvoid * const p_offset = 0
		)
		{
			glDrawElements( p_mode, p_count, p_type, p_offset );
		}

		inline void multiDrawElement(
			const uint32_t				 p_mode,
			const int32_t * const		 p_count,
			const uint32_t				 p_type,
			const GLvoid * const * const p_offset,
			const int32_t				 p_primcount
		)
		{
			glMultiDrawElements( p_mode, p_count, p_type, p_offset, p_primcount );
		}

	  private:
		uint32_t _id = GL_INVALID_INDEX;
	};

	template<>
	inline void VertexArray::setAttributeFormat<float>(
		const uint32_t p_attributeIndex,
		const int32_t  p_size,
		const uint32_t p_relativeOffset,
		const uint8_t  p_normalized
	) const
	{
		assert( glIsVertexArray( _id ) );

		glVertexArrayAttribFormat( _id, p_attributeIndex, p_size, GL_FLOAT, p_normalized, p_relativeOffset );
	}

	template<>
	inline void VertexArray::setAttributeFormat<double>(
		const uint32_t p_attributeIndex,
		const int32_t  p_size,
		const uint32_t p_relativeOffset,
		const uint8_t  p_normalized
	) const
	{
		assert( glIsVertexArray( _id ) );

		glVertexArrayAttribLFormat( _id, p_attributeIndex, p_size, GL_DOUBLE, p_relativeOffset );
	}

	template<>
	inline void VertexArray::setAttributeFormat<uint>(
		const uint32_t p_attributeIndex,
		const int32_t  p_size,
		const uint32_t p_relativeOffset,
		const uint8_t  p_normalized
	) const
	{
		assert( glIsVertexArray( _id ) );

		glVertexArrayAttribIFormat( _id, p_attributeIndex, p_size, GL_UNSIGNED_INT, p_relativeOffset );
	}

	template<>
	inline void VertexArray::setAttributeFormat<int>(
		const uint32_t p_attributeIndex,
		const int32_t  p_size,
		const uint32_t p_relativeOffset,
		const uint8_t  p_normalized
	) const
	{
		assert( glIsVertexArray( _id ) );

		glVertexArrayAttribIFormat( _id, p_attributeIndex, p_size, GL_INT, p_relativeOffset );
	}
} // namespace VTX::Renderer::Context::GL

#endif // __VTX_GL_VERETX_ARRAY__
