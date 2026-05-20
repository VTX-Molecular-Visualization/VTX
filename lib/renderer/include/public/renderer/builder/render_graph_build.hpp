#ifndef __VTX_RENDERER_BUILDER_RENDER_GRAPH_BUILD__
#define __VTX_RENDERER_BUILDER_RENDER_GRAPH_BUILD__

#include "renderer/geometry/geometries.hpp"
#include "renderer/graph_builder.hpp"
#include "renderer/layout/layouts.hpp"

namespace VTX::Renderer::Builder
{
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

	struct DefaultRenderGraph
	{
		static GraphBuilder build( const PipelineConfig &, const Layouts &, const Geometries & );
	};
} // namespace VTX::Renderer::Builder

#endif
