#ifndef __VTX_RENDERER_BUILDER_RENDER_GRAPH_BUILD__
#define __VTX_RENDERER_BUILDER_RENDER_GRAPH_BUILD__

#include "renderer/geometry/geometries.hpp"
#include "renderer/graph_builder.hpp"
#include "renderer/layout/layouts.hpp"
#include "renderer/render_graph.hpp"

namespace VTX::Renderer::Builder
{
	struct DefaultRenderGraph
	{
		static GraphBuilder build(
			const RenderGraph::PipelineConfig &,
			const Layouts &,
			const Geometries &
		);
	};
} // namespace VTX::Renderer::Builder

#endif
