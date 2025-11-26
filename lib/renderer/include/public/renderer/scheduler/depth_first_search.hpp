#ifndef __VTX_RENDERER_SCHEDULER_DFS__
#define __VTX_RENDERER_SCHEDULER_DFS__

#include "renderer/descriptors.hpp"

namespace VTX::Renderer::Scheduler
{
	/**
	 * @brief https://fr.wikipedia.org/wiki/Algorithme_de_parcours_en_profondeur
	 */
	class DepthFirstSearch
	{
	  public:
		RenderQueue schedule( const Passes & p_passes, const Links & p_links, const Output & p_output );
	};
} // namespace VTX::Renderer::Scheduler

#endif
