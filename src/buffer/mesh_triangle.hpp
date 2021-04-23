#ifndef __VTX_BUFFER_MESH_TRIANGLE__
#define __VTX_BUFFER_MESH_TRIANGLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_buffer_opengl.hpp"
#include "color/rgb.hpp"
#include "define.hpp"
#include "renderer/gl/vertex_array.hpp"
#include <vector>

namespace VTX
{
	namespace Buffer
	{
		class MeshTriangle : public BaseBufferOpenGL
		{
		  public:
			MeshTriangle( OpenGLFunctions * const p_gl ) : BaseBufferOpenGL( p_gl ), _vao( p_gl ) {};
			~MeshTriangle() = default; /// TODO free buffers !

			void bind() override;
			void unbind() override;

			void setPositions( const std::vector<Vec3f> & );
			void setNormals( const std::vector<Vec3f> & );
			void setColors( const std::vector<Color::Rgb> & );
			void setVisibilities( const std::vector<ushort> & );
			void setIndices( const std::vector<uint> & );

		  protected:
			void _generate() override;
			void _free() override;

		  private:
			enum ATTRIBUTE_LOCATION
			{
				VERTEX_POSITION	  = 0,
				VERTEX_NORMAL	  = 1,
				VERTEX_COLOR	  = 2,
				VERTEX_VISIBILITY = 3,
			};

			GLuint					  _vboPositions	   = GL_INVALID_VALUE;
			GLuint					  _vboNormals	   = GL_INVALID_VALUE;
			GLuint					  _vboColors	   = GL_INVALID_VALUE;
			GLuint					  _vboVisibilities = GL_INVALID_VALUE;
			GLuint					  _ibo			   = GL_INVALID_VALUE;
			Renderer::GL::VertexArray _vao;
		};
	} // namespace Buffer
} // namespace VTX
#endif
