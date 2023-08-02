#ifndef __VTX_RENDERER_RENDER_GRAPH__
#define __VTX_RENDERER_RENDER_GRAPH__

#include "context/concept_context.hpp"
#include "scheduler/concept_scheduler.hpp"

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

			// Check input exist and free.
			assert( passIn.inputs.contains( p_channel ) );
			assert( passIn.inputs[ p_channel ].source == nullptr );

			// Check descriptor compatibility.

			// Create link.
			passIn.inputs[ p_channel ].source = &passOut.output;

			// Update adjacent map.
			// 			if ( _adjacentMap.find( &passOut ) == _adjacentMap.end() )
			// 			{
			// 				_adjacentMap[ &passOut ] = std::vector<Pass *>();
			// 			}
			// 			_adjacentMap[ &passOut ].push_back( &passIn );

			return true;
		}

		inline void setup()
		{
			// Build ajdacent map.
			std::map<const Pass * const, std::vector<Pass *>> adjacentMap;

			// Topological sort with adjacent map.

			// Queue.
			Scheduler::RenderQueue queue = _scheduler->schedule( _passes );

			// TODO: create resources.
		}

		// Debug purposes only.
		const Passes & getPasses() const { return _passes; }

	  private:
		std::unique_ptr<C> _context;
		std::unique_ptr<S> _scheduler;

		Ressources _resources;
		Passes	   _passes;

		// 		using AdjacentMap = std::map<const Pass * const, std::vector<Pass *>>;
		// 		AdjacentMap _adjacentMap;
	};

} // namespace VTX::Renderer
#endif
