#ifndef __VTX_RENDERER_SCHEDULER_DFS__
#define __VTX_RENDERER_SCHEDULER_DFS__

#include "concept_scheduler.hpp"

namespace VTX::Renderer::Scheduler
{

	class DepthFirstSearch
	{
	  public:
		RenderQueue schedule( const Renderer::Passes & p_passes ) { return {}; }

	  private:
	};
} // namespace VTX::Renderer::Scheduler

#endif
