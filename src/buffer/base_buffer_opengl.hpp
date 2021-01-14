#ifndef __VTX_BASE_BUFFER_OPENGL__
#define __VTX_BASE_BUFFER_OPENGL__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "generic/base_opengl.hpp"

namespace VTX
{
	namespace Buffer
	{
		class BaseBufferOpenGL : public Generic::BaseOpenGL
		{
		  public:
			BaseBufferOpenGL( OpenGLFunctions * const p_gl ) : BaseOpenGL( p_gl ) {}
			virtual ~BaseBufferOpenGL() {};

			virtual void generate()
			{
				gl()->glCreateBuffers( 1, &_vboAABB );
				gl()->glCreateBuffers( 1, &_iboAABB );

				gl()->glCreateVertexArrays( 1, &_vaoAABB );
				
				gl()->glVertexArrayElementBuffer( _vaoAABB, _iboAABB );

				gl()->glEnableVertexArrayAttrib( _vaoAABB, ATTRIBUTE_AABB_LOCATION::AABB_CENTER);
				gl()->glVertexArrayVertexBuffer(
					_vaoAABB, ATTRIBUTE_AABB_LOCATION::AABB_CENTER, _vboAABB, 0, sizeof( Vec3f ) );
				gl()->glVertexArrayAttribFormat(
					_vaoAABB, ATTRIBUTE_AABB_LOCATION::AABB_CENTER, 3, GL_FLOAT, GL_FALSE, 0 );
				gl()->glVertexArrayAttribBinding(
					_vaoAABB, ATTRIBUTE_AABB_LOCATION::AABB_CENTER, ATTRIBUTE_AABB_LOCATION::AABB_CENTER );

				_generate();
			}

			virtual void free()
			{
				if ( _vaoAABB != GL_INVALID_VALUE )
				{					
					gl()->glDisableVertexAttribArray( ATTRIBUTE_AABB_LOCATION::AABB_CENTER );

					gl()->glDeleteVertexArrays( 1, &_vaoAABB );
				}

				if ( _vboAABB != GL_INVALID_VALUE )
				{
					gl()->glDeleteBuffers( 1, &_vboAABB );
				}
				if ( _iboAABB != GL_INVALID_VALUE )
				{
					gl()->glDeleteBuffers( 1, &_iboAABB );
				}

				_free();
			}

			virtual void bindAABB()
			{
				gl()->glBindVertexArray( _vaoAABB );
			}

			virtual void unbindAABB()
			{
				gl()->glBindVertexArray( 0 );
			}

			void setAABBCorners( const std::vector<Vec3f> & p_positions )
			{
				gl()->glNamedBufferData( _vboAABB, sizeof( Vec3f ) * GLsizei( p_positions.size() ), p_positions.data(), GL_STATIC_DRAW );
			}

			void setAABBIndices( const std::vector<uint> & p_indices )
			{
				gl()->glNamedBufferData( _iboAABB, sizeof( uint ) * GLsizei( p_indices.size() ), p_indices.data(), GL_STATIC_DRAW );
			}

			virtual void bind()	  = 0;
			virtual void unbind() = 0;

		  protected:
			virtual void _generate() = 0;
			virtual void _free()	 = 0;

		  private:
			enum ATTRIBUTE_AABB_LOCATION
			{
				AABB_CENTER = 0
			};

			GLuint _vboAABB = GL_INVALID_VALUE;
			GLuint _iboAABB = GL_INVALID_VALUE;
			GLuint _vaoAABB = GL_INVALID_VALUE;
		};
	} // namespace Buffer
} // namespace VTX
#endif
