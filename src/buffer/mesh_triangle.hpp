#ifndef __VTX_BUFFER_MESH_TRIANGLE__
#define __VTX_BUFFER_MESH_TRIANGLE__

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
			MeshTriangle()	= default;
			~MeshTriangle() = default;

			void generate() override;

			void setPositions( const std::vector<Vec3f> & );
			void setNormals( const std::vector<Vec3f> & );
			void setColors( const std::vector<Color::Rgb> & );
			void setVisibilities( const std::vector<uint> & );
			void setIndices( const std::vector<uint> & );

		  private:
			enum ATTRIBUTE_LOCATION
			{
				VERTEX_POSITION	  = 0,
				VERTEX_NORMAL	  = 1,
				VERTEX_COLOR	  = 2,
				VERTEX_VISIBILITY = 3,
			};

			Renderer::GL::Buffer _vboPositions	  = Renderer::GL::Buffer();
			Renderer::GL::Buffer _vboNormals	  = Renderer::GL::Buffer();
			Renderer::GL::Buffer _vboColors		  = Renderer::GL::Buffer();
			Renderer::GL::Buffer _vboVisibilities = Renderer::GL::Buffer();
			Renderer::GL::Buffer _ibo			  = Renderer::GL::Buffer();
		};
	} // namespace Buffer
} // namespace VTX
#endif
