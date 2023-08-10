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
			//_context   = std::make_unique<C>();
			//_scheduler = std::make_unique<S>();
		}

		inline void addPass( const std::string & p_name, Pass p_pass )
		{
			_passes.emplace( p_name, std::move( p_pass ) );
		}

		bool addLink( const std::string & p_passSrc,
					  const std::string & p_passDest,
					  const E_CHANNEL &	  p_channelSrc	= E_CHANNEL::COLOR_0,
					  const E_CHANNEL &	  p_channelDest = E_CHANNEL::COLOR_0 )
		{
			// Check pass exists.
			assert( _passes.contains( p_passSrc ) );
			assert( _passes.contains( p_passDest ) );

			Pass & passSrc	= _passes[ p_passSrc ];
			Pass & passDest = _passes[ p_passDest ];

			// Check IO existence.
			assert( passSrc.outputs.contains( p_channelSrc ) );
			assert( passDest.inputs.contains( p_channelDest ) );

			// Check input is free.
			if ( std::find_if( _links.begin(),
							   _links.end(),
							   [ & ]( const Link & p_element )
							   { return p_element.dest == &passDest && p_element.channelDest == p_channelDest; } )
				 != _links.end() )
			{
				VTX_WARNING( "Channel {} from pass {} is already used", uint( p_channelDest ), p_passDest );
				return false;
			}

			// Check descriptors compatibility.
			// 			if ( passIn.inputs[ p_channel ].desc != passOut.output.desc )
			// 			{
			// 				return false;
			// 			}

			// Create link.
			_links.push_back( { &passSrc, &passDest, p_channelSrc, p_channelDest } );

			return true;
		}

		bool setup()
		{
			// Queue.
			try
			{
				Scheduler::RenderQueue queue = _scheduler.schedule( _passes, _links );
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
		C _context;
		S _scheduler;

		// Ressources _resources;
		Passes _passes;
		Links  _links;
	};

} // namespace VTX::Renderer
#endif
