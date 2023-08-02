#ifndef __VTX_RENDERER_RENDER_GRAPH__
#define __VTX_RENDERER_RENDER_GRAPH__

#include "context/concept_context.hpp"
#include "scheduler/concept_scheduler.hpp"
#include "struct_link.hpp"
#include "struct_pass.hpp"
#include "struct_ressource.hpp"

namespace VTX::Renderer
{

	template<Context::Concept C, Scheduler::Concept S>
	class RenderGraph
	{
	  public:
		RenderGraph()
		{
			_context   = std::make_unique<C>();
			_scheduler = std::make_unique<S>();
		}

		inline void addPass( const std::string p_name, Pass p_pass ) { _passes.emplace( p_name, std::move( p_pass ) ); }

		bool addLink( const std::string &	  p_passOut,
					  const std::string &	  p_passIn,
					  const E_INPUT_CHANNEL & p_channel = E_INPUT_CHANNEL::COLOR_0 )
		{
			// Check pass exists.
			assert( _passes.contains( p_passOut ) );
			assert( _passes.contains( p_passIn ) );

			Pass & passOut = _passes[ p_passOut ];
			Pass & passIn  = _passes[ p_passIn ];

			// Check input exists and is free.
			assert( passIn.inputs.contains( p_channel ) );

			// Check link exists?

			// Check descriptor compatibility.

			// Create link.
			_links.push_back( { &passOut, &passIn, p_channel } );

			return true;
		}

		void setup()
		{
			// Topological sort with adjacent map.

			// Queue.
			Scheduler::RenderQueue queue = _scheduler->schedule( _passes );

			// TODO: create resources.
		}

		// Debug purposes only.
		inline const Passes & getPasses() const { return _passes; }
		inline const Links &  getLinks() const { return _links; }

	  private:
		std::unique_ptr<C> _context;
		std::unique_ptr<S> _scheduler;

		Ressources _resources;
		Passes	   _passes;
		Links	   _links;
	};

} // namespace VTX::Renderer
#endif
