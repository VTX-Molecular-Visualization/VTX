#ifndef __VTX_RENDERER_RENDER_GRAPH__
#define __VTX_RENDERER_RENDER_GRAPH__

// #include "context/concept.hpp"
#include "renderer/descriptors.hpp"
#include "renderer/graph_builder.hpp"
#include <util/exceptions.hpp>
#include <util/logger.hpp>

namespace VTX::Renderer
{
	/**
	 * @brief Ordered list of passes for execution.
	 */
	using RenderQueue = std::vector<const Pass *>;

	/**
	 * @brief A graph with nodes (passes) and links.
	 */
	class RenderGraph
	{
	  public:
		/**
		 * @brief Default pipeline configuration.
		 */
		struct PipelineConfig
		{
			bool enableSSAO		 = false;
			bool enableOutline	 = false;
			bool enableSelection = false;
		};

		/**
		 * @brief Accessors.
		 */
		inline const Resources & getResources() const { return _resources; }
		inline const PassList &	 getPasses() const { return _passes; }

		/**
		 * @brief Validate graph and build render queue.
		 * @return
		 */
		const RenderQueue build();

		/**
		 * @brief Add data from builder.
		 */
		void add( const GraphBuilder & );

		/**
		 * @brief Set data from builder.
		 */
		void set( GraphBuilder && );

		/**
		 * @brief Clear graph.
		 */
		void clear();

		/**
		 * @brief Create a default pipeline from config.
		 */
		void createDefaultPipeline( const PipelineConfig & );

	  private:
		Resources _resources;
		PassList  _passes;
	};

} // namespace VTX::Renderer
#endif
