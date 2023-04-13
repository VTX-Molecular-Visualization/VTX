#ifndef __VTX_BASE_BUFFER_OPENGL__
#define __VTX_BASE_BUFFER_OPENGL__

#include <util/constants.hpp>
#include <util/types.hpp>
#include "old_app/generic/base_opengl.hpp"
#include "old_app/renderer/gl/vertex_array.hpp"

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

			inline void generate()
			{
				makeContextCurrent();
				_generate();
				doneContextCurrent();
			}

			virtual void bind() { _vao.bind(); }
			virtual void unbind() { _vao.unbind(); }

		  protected:
			Renderer::GL::VertexArray _vao = Renderer::GL::VertexArray();

			virtual void _generate() = 0;

			template<typename T>
			inline void _updateBuffer( Renderer::GL::Buffer &	   p_buffer,
									   const std::vector<T> &	   p_vector,
									   Renderer::GL::Buffer::Usage p_usage = Renderer::GL::Buffer::Usage::STATIC_DRAW )
			{
				makeContextCurrent();
				p_buffer.set( p_vector, p_usage );
				doneContextCurrent();
			}
		};
	} // namespace Buffer
} // namespace VTX
#endif
