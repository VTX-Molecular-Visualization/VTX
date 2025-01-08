#ifndef __VTX_RENDERER_RENDER_GRAPH__
#define __VTX_RENDERER_RENDER_GRAPH__

#include "context/concept.hpp"
#include "scheduler/concept_scheduler.hpp"
#include <util/exceptions.hpp>
#include <util/logger.hpp>
#include <util/variant.hpp>

namespace VTX::Renderer
{
	/**
	 * @brief A graph with nodes (passes) and links.
	 */
	class RenderGraph
	{
	  public:
		inline const Passes &		getPasses() const { return _passes; }
		inline const Links &		getLinks() const { return _links; }
		inline const RenderQueue &	getRenderQueue() const { return _renderQueue; }
		inline const Output * const getOutput() { return _output; }
		inline void					setOutput( const Output * const p_output ) { _output = p_output; }
		inline void					clean() { _renderQueue.clear(); }

		Pass * const addPass( const Pass & p_pass );
		void		 removePass( const Pass * const p_pass );

		bool addLink(
			Pass * const	   p_passSrc,
			Pass * const	   p_passDest,
			const E_CHAN_OUT & p_channelSrc	 = E_CHAN_OUT::COLOR_0,
			const E_CHAN_IN &  p_channelDest = E_CHAN_IN::_0
		);

		void removeLink( const Link * const p_link );

		/**
		 * @brief Creates a render queue from graph.
		 * @tparam S the scheduler to use.
		 * @return the ordonanced render queue.
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
			_renderQueue.clear();
			scheduler.schedule( _passes, _links, _output, _renderQueue );

			// Some checks.
			if ( _renderQueue.empty() )
			{
				throw GraphicException( "Render queue is empty" );
			}

			if ( _renderQueue.back()->outputs.size() != 1 )
			{
				// Useless?
				// throw GraphicException( "The output of the last pass must be unique" );
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

	  private:
		Passes		   _passes;
		Links		   _links;
		const Output * _output;

		/**
		 * @brief The computed render queue.
		 */
		RenderQueue _renderQueue;
	};

} // namespace VTX::Renderer
#endif
