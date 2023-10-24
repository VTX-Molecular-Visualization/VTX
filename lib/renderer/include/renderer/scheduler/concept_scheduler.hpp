#ifndef __VTX_RENDERER_SCHEDULER_CONCEPT__
#define __VTX_RENDERER_SCHEDULER_CONCEPT__

#include "renderer/descriptors.hpp"
#include <concepts>
#include <vector>

namespace VTX::Renderer::Scheduler
{

	template<typename S>
	concept Concept = requires(
		S					 p_scheduler,
		Passes &			 p_passes,
		const Links &		 p_links,
		const Output * const p_output,
		RenderQueue &		 p_outRenderQueue
	) {
		{
			p_scheduler.schedule( p_passes, p_links, p_output, p_outRenderQueue )
		} -> std::same_as<void>;
	};
} // namespace VTX::Renderer::Scheduler

#endif
