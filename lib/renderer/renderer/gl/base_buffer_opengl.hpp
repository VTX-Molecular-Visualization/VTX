#ifndef __VTX_RENDERER_BASE_BUFFER_OPENGL__
#define __VTX_RENDERER_BASE_BUFFER_OPENGL__

#include "wrapper/vertex_array.hpp"
#include <vector>

namespace VTX::Renderer::GL
{

	class BaseBufferOpenGL
	{
	  public:
		BaseBufferOpenGL()			= default;
		virtual ~BaseBufferOpenGL() = default;

		virtual const Renderer::GL::VertexArray & getVao() const { return _vao; }

		inline void generate() { _generate(); }

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
			p_buffer.set( p_vector, p_usage );
		}
	};

} // namespace VTX::Renderer::GL
#endif
