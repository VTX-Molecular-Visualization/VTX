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

			virtual const Renderer::GL::VertexArray & getVao() const = 0;

			virtual void generate() = 0;
			virtual void bind()		= 0;
			virtual void unbind()	= 0;
		};
	} // namespace Buffer
} // namespace VTX
#endif
