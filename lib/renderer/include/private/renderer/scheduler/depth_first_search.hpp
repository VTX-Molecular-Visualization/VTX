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
		void schedule(
			Passes &			 p_passes,
			const Links &		 p_links,
			const Output * const p_output,
			RenderQueue &		 p_outRenderQueue
		);

	  private:
		void _depthFirstSearch(
			Passes &								 p_passes,
			const std::vector<std::vector<size_t>> & p_adjacencyLists,
			const size_t							 p_index,
			std::vector<bool> &						 p_visited,
			std::vector<bool> &						 p_onStack,
			bool &									 p_isCyclic,
			std::vector<size_t> &					 p_sorted
		);
	};
} // namespace VTX::Renderer::Scheduler

#endif
