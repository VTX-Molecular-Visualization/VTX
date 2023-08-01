#ifndef __VTX_RENDERER_SCHEDULER_CONCEPT__
#define __VTX_RENDERER_SCHEDULER_CONCEPT__

#include "renderer/struct_render_pass.hpp"
#include <concepts>
#include <vector>

namespace VTX::Renderer::Scheduler
{
	using ListCommand = std::vector<void *>;

	template<typename S>
	concept Concept = requires( S p_scheduler, Renderer::ListPasses p_passes ) {
		{
			p_scheduler.schedule( p_passes )
		} -> std::same_as<ListCommand>;
	};
} // namespace VTX::Renderer::Scheduler

#endif
