#ifndef __VTX_RENDERER_BUILDER_RENDER_GRAPH_BUILD__
#define __VTX_RENDERER_BUILDER_RENDER_GRAPH_BUILD__

#include "renderer/context/context_wrapper.hpp"
#include "renderer/geometry/geometries.hpp"
#include "renderer/graphics_config.hpp"
#include "renderer/graph_builder.hpp"
#include "renderer/layout/layouts.hpp"
#include "renderer/render_graph.hpp"
#include <optional>

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

	struct RenderGraphRuntime
	{
		static bool refreshGraph(
			const GraphicsConfig &,
			std::optional<PipelineConfig> &,
			RenderGraph &,
			Desc::RenderQueue &,
			const Layouts &,
			const Geometries &
		);

		static bool syncGeometryChunks( RenderGraph &, const Geometries & );

		static void bindExternalPasses( Context::ContextWrapper &, uintptr_t p_function, uintptr_t p_context );

		static void markSESDirty( Context::ContextWrapper &, const Geometries & );

		static void rebuildCommandBuffer(
			Context::ContextWrapper &,
			const Desc::RenderQueue &,
			const Desc::Resources &,
			uintptr_t p_externalFunction,
			uintptr_t p_externalContext
		);
	};
} // namespace VTX::Renderer::Builder

#endif
