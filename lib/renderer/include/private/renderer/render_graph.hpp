#ifndef __VTX_RENDERER_RENDER_GRAPH__
#define __VTX_RENDERER_RENDER_GRAPH__

#include "context/concept_context.hpp"
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

		template<Scheduler::Concept S>
		const RenderQueue & build()
		{
			VTX_DEBUG( "{}", "Building render graph..." );

			// Check ouptut.
			if ( _output == nullptr )
			{
				throw GraphicException( "No output defined" );
			}

			// Compute queue with scheduler.
			S scheduler;
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

			// Create context.
			//_context.set<C>( p_width, p_height, p_shaderPath, p_loader );
			// if ( _context == nullptr )
			//{
			//_context = std::make_unique<C>( p_width, p_height, p_shaderPath, p_loader );
			//}

			// Generate instructions.
			/*
			try
			{
				VTX_DEBUG( "{}", "Generating instructions..." );
				_context.build(
					_renderQueue, _links, p_output, _globalData, p_outInstructions, p_outInstructionsDurationRanges
				);
				VTX_DEBUG( "{}", "Generating instructions... done" );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Can not generate instructions: {}", p_e.what() );
				p_outInstructions.clear();
				return nullptr;
			}

			VTX_DEBUG( "{} instructions generated", p_outInstructions.size() );
			VTX_DEBUG( "{}", "Building render graph... done" );
			*/

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

		RenderQueue _renderQueue;
	};

} // namespace VTX::Renderer
#endif
