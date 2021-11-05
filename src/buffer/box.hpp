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

		const Renderer::GL::VertexArray & getVao() const override { return _vao; }

		void generate() override;
		void bind() override;
		void unbind() override;

		void setCorners( const std::vector<Vec3f> & );
		void setIndices( const std::vector<uint> & );

	  private:
		enum ATTRIBUTE_LOCATION
		{
			AABB_CORNER = 0
		};

		Renderer::GL::VertexArray _vao = Renderer::GL::VertexArray();
		Renderer::GL::Buffer	  _vbo = Renderer::GL::Buffer();
		Renderer::GL::Buffer	  _ibo = Renderer::GL::Buffer();
	};
} // namespace VTX::Buffer

#endif
