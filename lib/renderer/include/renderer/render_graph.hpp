#ifndef __VTX_RENDERER_RENDER_GRAPH__
#define __VTX_RENDERER_RENDER_GRAPH__

#include "context/concept_context.hpp"
#include "scheduler/concept_scheduler.hpp"
#include "struct_link.hpp"
#include "struct_pass.hpp"
#include "struct_ressource.hpp"
#include <util/logger.hpp>

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

		inline void addPass( const std::string & p_name, Pass p_pass )
		{
			_passes.emplace( p_name, std::move( p_pass ) );
		}

		bool addLink( const std::string &	  p_passOut,
					  const std::string &	  p_passIn,
					  const E_INPUT_CHANNEL & p_channel = E_INPUT_CHANNEL::COLOR_0 )
		{
			// Check pass exists.
			assert( _passes.contains( p_passOut ) );
			assert( _passes.contains( p_passIn ) );

			Pass & passOut = _passes[ p_passOut ];
			Pass & passIn  = _passes[ p_passIn ];

			// Check input existence.
			assert( passIn.inputs.contains( p_channel ) );

			// Check input is free.
			if ( std::find_if( _links.begin(),
							   _links.end(),
							   [ & ]( const Link & p_element )
							   { return p_element.destination == &passIn && p_element.channel == p_channel; } )
				 != _links.end() )
			{
				VTX_WARNING( "Channel {} from pass {} is already used", uint( p_channel ), p_passIn );
				return false;
			}

			// Check descriptors compatibility.
			// 			if ( passIn.inputs[ p_channel ].desc != passOut.output.desc )
			// 			{
			// 				return false;
			// 			}

			// Create link.
			_links.push_back( { &passOut, &passIn, p_channel } );

			return true;
		}

		bool setup()
		{
			// Queue.
			try
			{
				Scheduler::RenderQueue queue = _scheduler->schedule( _passes, _links );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Can not build render graph: {}", p_e.what() );
				return false;
			}
			// TODO: create resources.

			return true;
		}

		// Debug purposes only.
		inline const Passes & getPasses() const { return _passes; }
		inline const Links &  getLinks() const { return _links; }

	  private:
		std::unique_ptr<C> _context;
		std::unique_ptr<S> _scheduler;

		// Ressources _resources;
		Passes _passes;
		Links  _links;
	};

} // namespace VTX::Renderer
#endif
