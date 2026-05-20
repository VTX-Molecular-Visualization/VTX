#ifndef __VTX_RENDERER_RENDER_GRAPH__
#define __VTX_RENDERER_RENDER_GRAPH__

#include "renderer/descriptors.hpp"
#include "renderer/graph_builder.hpp"
#include <span>

namespace VTX::Renderer
{

	/**
	 * @brief A graph with nodes (passes) and links.
	 */
	class RenderGraph
	{
	  public:
		/**
		 * @brief Accessors.
		 */
		inline const Desc::Resources & getResources() const { return _resources; }

		inline const Desc::PassList & getPasses() const { return _passes; }

		/**
		 * @brief Update the active physical chunks for a geometry resource.
		 */
		bool setGeometryChunks( const Desc::Key &, std::span<const uint32_t> );

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

	  private:
		Desc::Resources _resources;
		Desc::PassList	_passes;
	};

} // namespace VTX::Renderer
#endif
