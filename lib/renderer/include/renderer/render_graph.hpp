#ifndef __VTX_RENDERER_RENDER_GRAPH__
#define __VTX_RENDERER_RENDER_GRAPH__

#include "context/concept_context.hpp"
#include "scheduler/concept_scheduler.hpp"
#include <util/logger.hpp>
#include <util/variant.hpp>

namespace VTX::Renderer
{

	template<Context::Concept C, Scheduler::Concept S>
	class RenderGraph
	{
	  public:
		RenderGraph() = default;
		~RenderGraph() { _clear(); }

		inline RenderQueue &  getRenderQueue() { return _renderQueue; }
		inline const Output * getOutput() { return _output; }
		inline void			  setOutput( const Output * const p_output ) { _output = p_output; }

		inline Pass * const addPass( const Pass & p_pass )
		{
			_passes.emplace_back( std::make_unique<Pass>( p_pass ) );
			return _passes.back().get();
		}

		bool addLink( Pass * const			   p_passSrc,
					  Pass * const			   p_passDest,
					  const E_CHANNEL_OUTPUT & p_channelSrc	 = E_CHANNEL_OUTPUT::COLOR_0,
					  const E_CHANNEL_INPUT &  p_channelDest = E_CHANNEL_INPUT::_0 )
		{
			// Check I/O existence.
			assert( p_passSrc->outputs.contains( p_channelSrc ) );
			assert( p_passDest->inputs.contains( p_channelDest ) );

			// Check input is free.
			if ( std::find_if( _links.begin(),
							   _links.end(),
							   [ &p_passDest, &p_channelDest ]( const std::unique_ptr<Link> & p_element ) {
								   return p_element.get()->dest == p_passDest
										  && p_element.get()->channelDest == p_channelDest;
							   } )
				 != _links.end() )
			{
				VTX_WARNING( "Channel {} from pass {} is already in use", uint( p_channelDest ), p_passDest->name );
				return false;
			}

			// TODO: Check descriptors compatibility.
			// 			if ( passIn.inputs[ p_channel ].desc != passOut.output.desc )
			// 			{
			// 				return false;
			// 			}

			// Create link.
			_links.emplace_back(
				std::make_unique<Link>( Link { p_passSrc, p_passDest, p_channelSrc, p_channelDest } ) );

			return true;
		}

		void removeLink( const Link * const p_link )
		{
			// TODO: is link deleted?
			std::erase_if( _links, [ &p_link ]( const std::unique_ptr<Link> & p_e ) { return p_e.get() == p_link; } );
		}

		bool setup( void * const	 p_loader,
					const size_t	 p_width,
					const size_t	 p_height,
					const FilePath & p_shaderPath,
					const Handle	 p_output = 0 )
		{
			// Clean all.
			_clear();

			VTX_DEBUG( "{}", "Building render graph..." );

			// Check ouptut.
			if ( _output == nullptr )
			{
				VTX_ERROR( "{}", "No output defined" );
				return false;
			}

			// Compute queue with scheduler.
			try
			{
				_scheduler.schedule( _passes, _links, _renderQueue );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Can not build render queue: {}", p_e.what() );
				return false;
			}

			// Check last render queue item has the defined output.
			const Outputs & outputs = _renderQueue.back()->outputs;
			const auto		it		= std::find_if( outputs.begin(),
											outputs.end(),
											[ this ]( const std::pair<E_CHANNEL_OUTPUT, Output> & p_e )
											{ return &( p_e.second ) == _output; } );

			if ( it == outputs.end() )
			{
				VTX_ERROR( "{}", "Last render queue item has not the defined output" );
				return false;
			}

			// Create context.
			_context = std::make_unique<C>( p_width, p_height, p_shaderPath, p_loader );

			// Generate instructions.
			try
			{
				VTX_DEBUG( "{}", "Generating instructions..." );
				_context->build( _renderQueue, _links, p_output, _instructions );
				VTX_DEBUG( "{}", "Generating instructions... done" );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Can not generate instructions: {}", p_e.what() );
				return false;
			}

			VTX_DEBUG( "{}", "Building render graph... done" );
			return true;
		}

		void resize( const size_t p_width, const size_t p_height )
		{
			using namespace Context;

			assert( _context != nullptr );
			_context->resize( p_width, p_height );
		}

		void render()
		{
			// TODO: Move to renderer?
			// Execute instructions.
			for ( Instruction & instruction : _instructions )
			{
				instruction();
			}
		}

		// Debug purposes only.
		inline Passes & getPasses() { return _passes; }
		inline Links &	getLinks() { return _links; }

	  private:
		S				   _scheduler;
		RenderQueue		   _renderQueue;
		std::unique_ptr<C> _context;

		const Output * _output;
		Passes		   _passes;
		Links		   _links;
		Instructions   _instructions;

		void _clear()
		{
			_renderQueue.clear();
			_instructions.clear();
			_context.reset( nullptr );
		}
	};

} // namespace VTX::Renderer
#endif
