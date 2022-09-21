#ifndef __VTX_BUFFER_MESH_TRIANGLE__
#define __VTX_BUFFER_MESH_TRIANGLE__

#include "base_buffer_opengl.hpp"
#include "color/rgb.hpp"
#include "define.hpp"
#include "model/base_model.hpp"
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
			void setSelections( const std::vector<uint> & );
			void setIds( const std::vector<Model::ID> & );
			void setIndices( const std::vector<uint> & );

		  private:
			enum ATTRIBUTE_LOCATION
			{
				VERTEX_POSITION	  = 0,
				VERTEX_NORMAL	  = 1,
				VERTEX_COLOR	  = 2,
				VERTEX_VISIBILITY = 3,
				VERTEX_SELECTION  = 4,
				VERTEX_ID		  = 5,
			};

			Renderer::GL::BufferData _vboPositions	  = Renderer::GL::BufferData();
			Renderer::GL::BufferData _vboNormals	  = Renderer::GL::BufferData();
			Renderer::GL::BufferData _vboColors		  = Renderer::GL::BufferData();
			Renderer::GL::BufferData _vboVisibilities = Renderer::GL::BufferData();
			Renderer::GL::BufferData _vboSelections	  = Renderer::GL::BufferData();
			Renderer::GL::BufferData _vboIds		  = Renderer::GL::BufferData();
			Renderer::GL::BufferData _ibo			  = Renderer::GL::BufferData();
		};
	} // namespace Buffer
} // namespace VTX
#endif
