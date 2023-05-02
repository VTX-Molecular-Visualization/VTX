#ifndef __VTX_APP_RENDER_BASE_BUFFER_OPENGL__
#define __VTX_APP_RENDER_BASE_BUFFER_OPENGL__

#include "app/old_app/generic/base_opengl.hpp"
#include "app/render/renderer/gl/vertex_array.hpp"
#include <util/constants.hpp>
#include <util/types.hpp>

namespace VTX::App::Render::Buffer
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
} // namespace VTX::App::Render::Buffer
#endif
