#ifndef __VTX_BUFFER_BOX__
#define __VTX_BUFFER_BOX__

#include "base_buffer_opengl.hpp"

namespace VTX::Buffer
{
	class Box : public BaseBufferOpenGL
	{
	  public:
		Box()  = default;
		~Box() = default;

		void generate() override;

		void setCorners( const std::vector<Vec3f> & );
		void setIndices( const std::vector<uint> & );

	  private:
		enum ATTRIBUTE_LOCATION
		{
			AABB_CORNER = 0
		};

		Renderer::GL::BufferData _vbo = Renderer::GL::BufferData();
		Renderer::GL::BufferData _ibo = Renderer::GL::BufferData();
	};
} // namespace VTX::Buffer

#endif
