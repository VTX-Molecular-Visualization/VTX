#ifndef __VTX_RENDERE_RENDERER__
#define __VTX_RENDERE_RENDERER__

#include "context/opengl_45.hpp"
#include "render_graph.hpp"
#include "scheduler/dept_first_search.hpp"

namespace VTX::Renderer
{
	class Renderer
	{
		using RenderGraphOpenGL45 = RenderGraph<Context::OpenGL45, Scheduler::DepthFirstSearch>;

	  public:
	  private:
		RenderGraphOpenGL45 _graph;
	};
} // namespace VTX::Renderer

#endif
