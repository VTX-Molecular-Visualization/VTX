#ifndef __VTX_RENDERER_RENDER_GRAPH__
#define __VTX_RENDERER_RENDER_GRAPH__

#include "context/concept.hpp"
#include "renderer/settings.hpp"
#include "scheduler/concept.hpp"
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
		};

		/**
		 * @brief Default pipeline passes.
		 */
		struct PipelinePasses
		{
			Pass * geo		 = nullptr;
			Pass * depth	 = nullptr;
			Pass * ssao		 = nullptr;
			Pass * blurX	 = nullptr;
			Pass * blurY	 = nullptr;
			Pass * shading	 = nullptr;
			Pass * outline	 = nullptr;
			Pass * selection = nullptr;
			Pass * fxaa		 = nullptr;
		};

		/**
		 * @brief Accessors.
		 */
		inline const Passes &		getPasses() const { return _passes; }
		inline const Links &		getLinks() const { return _links; }
		inline const Output * const getOutput() { return _output; }
		inline void					setOutput( const Output * const p_output ) { _output = p_output; }
		inline const RenderQueue &	getRenderQueue() const { return _renderQueue; }

		/**
		 * @brief Clear all graph data (passes and links).
		 */
		inline void clear()
		{
			_passes.clear();
			_links.clear();
			_output = nullptr;
			_renderQueue.clear();
		}

		/**
		 * @brief Add a pass to the graph (copied).
		 */
		Pass * const addPass( const Pass & p_pass );

		/**
		 * @brief Remove a given pass.
		 */
		void removePass( const Pass * const p_pass );

		/**
		 * @brief Connect two passes.
		 */
		bool addLink(
			Pass * const	   p_passSrc,
			Pass * const	   p_passDest,
			const E_CHAN_OUT & p_channelSrc	 = E_CHAN_OUT::COLOR_0,
			const E_CHAN_IN &  p_channelDest = E_CHAN_IN::_0
		);

		/**
		 * @brief Disconnect two passes.
		 */
		void removeLink( const Link * const p_link );

		/**
		 * @brief Creates a render queue from current graph.
		 */
		template<Scheduler::Concept S>
		const RenderQueue & build()
		{
			// Check ouptut.
			if ( _output == nullptr )
			{
				throw GraphicException( "No output defined" );
			}

			// Compute queue with scheduler.
			S scheduler;
			_renderQueue = scheduler.schedule( _passes, _links, *_output );

			// Some checks.
			// TODO: check.
			if ( _renderQueue.empty() )
			{
				throw GraphicException( "Render queue is empty" );
			}
			// TODO: check.
			if ( _renderQueue.back()->outputs.size() != 1 )
			{
				throw GraphicException( "The output of the last pass must be unique" );
			}

			std::string str = "Passes: ";
			for ( const Pass * const pass : _renderQueue )
			{
				str += pass->name + " -> ";
			}
			str += "Output";
			VTX_DEBUG( "{}", str );

			return _renderQueue;
		}

		/**
		 * @brief Create a default pipeline from config.
		 */
		PipelinePasses createDefaultPipeline( const PipelineConfig & );

	  private:
		Passes		   _passes;
		Links		   _links;
		const Output * _output = nullptr;
		RenderQueue	   _renderQueue;
	};

} // namespace VTX::Renderer
#endif
