#ifndef __VTX_RENDERER_SCHEDULER_CONCEPT__
#define __VTX_RENDERER_SCHEDULER_CONCEPT__

#include "renderer/struct_render_pass.hpp"
#include <concepts>
#include <vector>

namespace VTX::Renderer::Scheduler
{
	using RenderQueue = std::vector<Renderer::Pass *>;

	template<typename S>
	concept Concept = requires( S p_scheduler, Renderer::Passes p_passes ) {
		{
			p_scheduler.schedule( p_passes )
		} -> std::same_as<RenderQueue>;
	};
} // namespace VTX::Renderer::Scheduler

#endif
