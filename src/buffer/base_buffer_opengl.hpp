#ifndef __VTX_BASE_BUFFER_OPENGL__
#define __VTX_BASE_BUFFER_OPENGL__

#include "define.hpp"
#include "generic/base_opengl.hpp"
#include "renderer/gl/vertex_array.hpp"

namespace VTX
{
	namespace Buffer
	{
		class BaseBufferOpenGL : public Generic::BaseOpenGL
		{
		  public:
			BaseBufferOpenGL()			= default;
			virtual ~BaseBufferOpenGL() = default;

			virtual const Renderer::GL::VertexArray & getVao() const { return _vao; }

			virtual void generate() = 0;

			virtual void bind() { _vao.bind(); }
			virtual void unbind() { _vao.unbind(); }

		  protected:
			Renderer::GL::VertexArray _vao = Renderer::GL::VertexArray();
		};
	} // namespace Buffer
} // namespace VTX
#endif
