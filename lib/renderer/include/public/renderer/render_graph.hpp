#ifndef __VTX_RENDERER_RENDER_GRAPH__
#define __VTX_RENDERER_RENDER_GRAPH__

#include "renderer/descriptors.hpp"
#include "renderer/geometry/geometries.hpp"
#include "renderer/graph_builder.hpp"
#include "renderer/layout/layouts.hpp"
#include <util/exceptions.hpp>
#include <util/logger.hpp>

namespace VTX::Renderer
{

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

			bool operator==( const PipelineConfig & ) const = default;
		};

		/**
		 * @brief Accessors.
		 */
		inline const Desc::Resources & getResources() const { return _resources; }
		inline const Desc::PassList &  getPasses() const { return _passes; }

		/**
		 * @brief Validate graph and build render queue.
		 * @return
		 */
		const Desc::RenderQueue build();

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
		void createDefaultPipeline( const PipelineConfig &, const Layouts &, const Geometries & );

		/**
		 * @brief Get pipeline config.
		 */
		const std::optional<PipelineConfig> & getPipelineConfig() const { return _config; }

	  private:
		Desc::Resources _resources;
		Desc::PassList	_passes;

		std::optional<PipelineConfig> _config;
	};

} // namespace VTX::Renderer
#endif
