#ifndef __VTX_RENDERER_RENDER_GRAPH__
#define __VTX_RENDERER_RENDER_GRAPH__

#include "context/concept_context.hpp"
#include "scheduler/concept_scheduler.hpp"
#include "struct_link.hpp"
#include "struct_pass.hpp"
#include "struct_ressource.hpp"
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

		inline Scheduler::RenderQueue & getRenderQueue() { return _renderQueue; }
		inline const Pass::Output *		getOutput() { return _output; }
		inline void						setOutput( const Pass::Output * const p_output ) { _output = p_output; }

		inline Pass * const addPass( const Pass & p_pass )
		{
			_passes.emplace_back( std::make_unique<Pass>( p_pass ) );
			return _passes.back().get();
		}

		bool addLink( Pass * const		p_passSrc,
					  Pass * const		p_passDest,
					  const E_CHANNEL & p_channelSrc  = E_CHANNEL::COLOR_0,
					  const E_CHANNEL & p_channelDest = E_CHANNEL::COLOR_0 )
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

			// Check descriptors compatibility.
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
			std::erase_if( _links, [ &p_link ]( const std::unique_ptr<Link> & p_e ) { return p_e.get() == p_link; } );
		}

		bool setup( void * const	 p_loader,
					const size_t	 p_width,
					const size_t	 p_height,
					const FilePath & p_shaderPath,
					const uint		 p_output = 0 )
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
				S scheduler;
				scheduler.schedule( _passes, _links, _renderQueue );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Can not build render queue: {}", p_e.what() );
				return false;
			}

			// Create context.
			_context = std::make_unique<C>( p_width, p_height, p_shaderPath, p_loader );

			// Create resources.
			try
			{
				VTX_DEBUG( "{}", "Creating resources..." );
				_createResources( _renderQueue );
				VTX_DEBUG( "{}", "Creating resources... done" );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Can not create resources: {}", p_e.what() );
				return false;
			}

			// Generate instructions.
			try
			{
				VTX_DEBUG( "{}", "Generating instructions..." );
				_instructions.emplace_back( [ & ]() { _context->clear(); } );
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

			for ( Resource & resource : _resources )
			{
				if ( std::holds_alternative<DescAttachment>( resource.desc ) )
				{
					_context->resize( resource.id, std::get<DescAttachment>( resource.desc ) );
				}
			}
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
		using Instruction  = std::function<void()>;
		using Instructions = std::vector<Instruction>;

		Scheduler::RenderQueue _renderQueue;
		std::unique_ptr<C>	   _context;

		const Pass::Output * _output;
		Passes				 _passes;
		Links				 _links;
		Resources			 _resources;
		Instructions		 _instructions;

		void _createResources( const Scheduler::RenderQueue & p_queue )
		{
			using namespace Context;

			for ( const Pass * const pass : p_queue )
			{
				// Outputs.
				for ( const auto & [ channel, output ] : pass->outputs )
				{
					const DescIO & desc = output.desc;

					Handle id = -1;
					if ( std::holds_alternative<DescAttachment>( desc ) )
					{
						_context->create( id, std::get<DescAttachment>( desc ) );
					}
					else
					{
						throw std::runtime_error( "unknown descriptor type" );
					}
					assert( id != -1 );
					_resources.push_back( Resource { id, Util::Variant::cast( desc ) } );
				}

				// Programs.
				for ( const DescProgram & desc : pass->programs )
				{
					Handle id;
					_context->create( id, desc );
					_resources.push_back( Resource { id, desc } );
				}
			}
		}

		void _clear()
		{
			using namespace Context;

			for ( Resource & resource : _resources )
			{
				if ( std::holds_alternative<DescAttachment>( resource.desc ) )
				{
					_context->destroy( resource.id, std::get<DescAttachment>( resource.desc ) );
				}
				else if ( std::holds_alternative<DescProgram>( resource.desc ) )
				{
					_context->destroy( resource.id, std::get<DescProgram>( resource.desc ) );
				}
				else
				{
					throw std::runtime_error( "unknown descriptor type" );
				}
			}

			_resources.clear();
			_renderQueue.clear();
			_instructions.clear();
			_context.reset( nullptr );
		}
	};

} // namespace VTX::Renderer
#endif
