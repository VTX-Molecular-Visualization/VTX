#ifndef __VTX_RENDERER_SCHEDULER_CONCEPT__
#define __VTX_RENDERER_SCHEDULER_CONCEPT__

#include "renderer/descriptors.hpp"
#include <concepts>

namespace VTX::Renderer::Scheduler
{

	/**
	 * @brief The scheduler use graph links and nodes to compute the render queue.
	 */
	template<typename S>
	concept Concept
		= requires( S p_scheduler, const Passes & p_passes, const Links & p_links, const Output & p_output ) {
			  { p_scheduler.schedule( p_passes, p_links, p_output ) } -> std::same_as<RenderQueue>;
		  };
} // namespace VTX::Renderer::Scheduler

#endif
