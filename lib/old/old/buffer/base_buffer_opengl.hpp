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

			inline void generate()
			{
				_makeCurrent();
				_generate();
				_doneCurrent();
			}

			virtual void bind() { _vao.bind(); }
			virtual void unbind() { _vao.unbind(); }

		  protected:
			Renderer::GL::VertexArray _vao = Renderer::GL::VertexArray();

			virtual void _generate() = 0;

			template<typename T>
			inline void _updateBuffer( Renderer::GL::BufferData &	   p_buffer,
									   const std::vector<T> &		   p_vector,
									   Renderer::GL::BufferData::Usage p_usage
									   = Renderer::GL::BufferData::Usage::STATIC_DRAW )
			{
				_makeCurrent();
				p_buffer.set( p_vector, p_usage );
				_doneCurrent();
			}
		};
	} // namespace Buffer
} // namespace VTX
#endif
