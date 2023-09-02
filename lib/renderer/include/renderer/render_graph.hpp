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

			// Check I/O existence.
			assert( passSrc.outputs.contains( p_channelSrc ) );
			assert( passDest.inputs.contains( p_channelDest ) );

			// Check input is free.
			if ( std::find_if( _links.begin(),
							   _links.end(),
							   [ &passDest, &p_channelDest ]( const Link & p_element )
							   { return p_element.dest == &passDest && p_element.channelDest == p_channelDest; } )
				 != _links.end() )
			{
				VTX_WARNING( "Channel {} from pass {} is already in use", uint( p_channelDest ), p_passDest );
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

		// 		template<class... Ts>
		// 		struct overloaded : Ts...
		// 		{
		// 			using Ts::operator()...;
		// 		};

		bool setup( const size_t	 p_width,
					const size_t	 p_height,
					const FilePath & p_shaderPath,
					void *			 p_proc = nullptr )
		{
			VTX_DEBUG( "{}", "Building render graph..." );

			// Compute queue with scheduler.
			Scheduler::RenderQueue queue;
			try
			{
				S scheduler;
				scheduler.schedule( _passes, _links, queue );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Can not build render queue: {}", p_e.what() );
				return false;
			}

			// Create resources.
			// 			struct VisitorDescIO
			// 			{
			// 				void operator()( const DescAttachment & p_desc ) const { _context.create( p_desc ); }
			// 				void operator()( const DescStorage p_desc ) const { _context.create( p_desc ); }
			// 			};

			// Clean all?

			// Create context.
			_context = std::make_unique<C>( p_width, p_height, p_shaderPath, p_proc );
			try
			{
				VTX_DEBUG( "{}", "Creating resources..." );
				_createResources( queue );
				VTX_DEBUG( "{}", "Creating resources... done" );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Can not create resources: {}", p_e.what() );
				return false;
			}

			// Generate instructions.
			// TODO.

			VTX_DEBUG( "{}", "Building render graph... done" );

			return true;
		}

		void resize( const size_t p_width, const size_t p_height ) {}

		// Debug purposes only.
		inline const Passes & getPasses() const { return _passes; }
		inline const Links &  getLinks() const { return _links; }

	  private:
		using Instruction  = std::function<void()>;
		using Instructions = std::vector<Instruction>;

		std::unique_ptr<C> _context;

		Passes		 _passes;
		Links		 _links;
		Ressources	 _resources;
		Instructions _instructions;

		void _createResources( const Scheduler::RenderQueue & p_queue )
		{
			using namespace Context;

			for ( const Pass * const pass : p_queue )
			{
				// Outputs.
				for ( const auto & [ channel, output ] : pass->outputs )
				{
					const DescIO & desc = output.desc;

					Handle id;
					if ( std::holds_alternative<DescAttachment>( desc ) )
					{
						_context->create( std::get<DescAttachment>( desc ), id );
					}
					else if ( std::holds_alternative<DescStorage>( desc ) )
					{
						_context->create( std::get<DescStorage>( desc ), id );
					}
					else
					{
						throw std::runtime_error( "unknown descriptor type" );
					}
				}

				// Programs.
				for ( const auto & program : pass->programs )
				{
					Handle id;
					_context->create( program, id );
				}
			}
		}
	};

} // namespace VTX::Renderer
#endif
