#ifndef __VTX_RENDERER_BUILDER_RENDER_GRAPH_BUILD__
#define __VTX_RENDERER_BUILDER_RENDER_GRAPH_BUILD__

#include "renderer/context/context_wrapper.hpp"
#include "renderer/geometry/geometries.hpp"
#include "renderer/graph_builder.hpp"
#include "renderer/graphics_config.hpp"
#include "renderer/layout/layouts.hpp"
#include "renderer/render_graph.hpp"
#include <optional>
#include <util/types.hpp>

namespace VTX::Renderer::Builder
{
	/**
	 * @brief Default pipeline configuration.
	 */
	struct PipelineConfig
	{
		bool		  enableSSAO				= false;
		E_SSAO_METHOD ssaoMethod				= SSAO_METHOD_DEFAULT;
		float		  ssaoScale					= SSAO_SCALE_DEFAULT;
		E_SHADING	  shadingMode				= SHADING_MODE_DEFAULT;
		bool		  enableOutline				= false;
		bool		  enableSelection			= false;
		bool		  enableChromaticAberration = false;
		bool		  enablePixelize			= false;
		bool		  enableCRT					= false;
		bool		  enableEnvironment			= false;
		uint		  environmentFaceSize		= 1;

		bool operator==( const PipelineConfig & ) const = default;
	};

	struct DefaultRenderGraph
	{
		static GraphBuilder build( const PipelineConfig &, const Layouts &, const Geometries & );
	};

	struct RenderGraphRuntime
	{
		static PipelineConfig pipelineConfig( const GraphicsConfig & );

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
