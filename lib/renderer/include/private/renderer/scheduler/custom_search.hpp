#ifndef __VTX_RENDERER_SCHEDULER_CUSTOM__
#define __VTX_RENDERER_SCHEDULER_CUSTOM__

#include "renderer/descriptors.hpp"
#include <unordered_set>

namespace VTX::Renderer::Scheduler
{

	class CustomSearch
	{
	  public:
		void schedule(
			Passes &			 p_passes,
			const Links &		 p_links,
			const Output * const p_output,
			RenderQueue &		 p_outRenderQueue
		);

	  private:
		void _sort(
			const size_t							  p_currentPass,
			std::vector<Pass *> &					  p_passes,
			std::vector<std::unordered_set<size_t>> & p_adjacentList,
			RenderQueue &							  p_outRenderQueue
		);
	};
} // namespace VTX::Renderer::Scheduler

#endif
