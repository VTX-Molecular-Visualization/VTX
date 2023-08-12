#ifndef __VTX_RENDERER_SCHEDULER_CONCEPT__
#define __VTX_RENDERER_SCHEDULER_CONCEPT__

#include "renderer/struct_link.hpp"
#include "renderer/struct_pass.hpp"
#include <concepts>
#include <vector>

namespace VTX::Renderer::Scheduler
{
	using RenderQueue = std::vector<Pass *>;

	template<typename S>
	concept Concept = requires( S p_scheduler, Passes & p_passes, const Links & p_links, RenderQueue & p_queue ) {
		{
			p_scheduler.schedule( p_passes, p_links, p_queue )
		} -> std::same_as<void>;
	};
} // namespace VTX::Renderer::Scheduler

#endif
