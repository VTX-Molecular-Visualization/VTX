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
		~RenderGraph() { _clear(); }

		inline Scheduler::RenderQueue & getRenderQueue() { return _renderQueue; }
		inline const Pass::Output *		getOutput() { return _output; }
		inline void						setOutput( const Pass::Output * const p_output ) { _output = p_output; }

		inline void addPass( const Pass & p_pass ) { _passes.push_back( p_pass ); }

		bool addLink( Pass &			p_passSrc,
					  Pass &			p_passDest,
					  const E_CHANNEL & p_channelSrc  = E_CHANNEL::COLOR_0,
					  const E_CHANNEL & p_channelDest = E_CHANNEL::COLOR_0 )
		{
			// Check I/O existence.
			assert( p_passSrc.outputs.contains( p_channelSrc ) );
			assert( p_passDest.inputs.contains( p_channelDest ) );

			// Check input is free.
			if ( std::find_if( _links.begin(),
							   _links.end(),
							   [ &p_passDest, &p_channelDest ]( const Link & p_element )
							   { return p_element.dest == &p_passDest && p_element.channelDest == p_channelDest; } )
				 != _links.end() )
			{
				VTX_WARNING( "Channel {} from pass {} is already in use", uint( p_channelDest ), p_passDest.name );
				return false;
			}

			// Check descriptors compatibility.
			// 			if ( passIn.inputs[ p_channel ].desc != passOut.output.desc )
			// 			{
			// 				return false;
			// 			}

			// Create link.
			_links.push_back( { &p_passSrc, &p_passDest, p_channelSrc, p_channelDest } );

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
				_renderQueue = scheduler.schedule( _passes, _links );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Can not build render queue: {}", p_e.what() );
				return false;
			}

			// Create context.
			if ( _context == nullptr )
			{
				_context = std::make_unique<C>( p_width, p_height, p_shaderPath, p_proc );
			}

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
			// TODO.

			VTX_DEBUG( "{}", "Building render graph... done" );

			return true;
		}

		void resize( const size_t p_width, const size_t p_height ) {}

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

					Handle id;
					if ( std::holds_alternative<DescAttachment>( desc ) )
					{
						_context->create( std::get<DescAttachment>( desc ), id );
					}
					else
					{
						throw std::runtime_error( "unknown descriptor type" );
					}
					_resources.push_back( Resource { id, variant_cast( desc ) } );
				}

				// Programs.
				for ( const DescProgram & desc : pass->programs )
				{
					Handle id;
					_context->create( desc, id );
					_resources.push_back( Resource { id, desc } );
				}
			}
		}

		//////////////////////////
		template<class... Args>
		struct variant_cast_proxy
		{
			std::variant<Args...> v;

			template<class... ToArgs>
			operator std::variant<ToArgs...>() const
			{
				return std::visit( []( auto && arg ) -> std::variant<ToArgs...> { return arg; }, v );
			}
		};

		template<class... Args>
		auto variant_cast( const std::variant<Args...> & v ) -> variant_cast_proxy<Args...>
		{
			return { v };
		}
		//////////////////////////

		void _clear()
		{
			using namespace Context;

			_renderQueue.clear();
			for ( const Resource & resource : _resources )
			{
				if ( std::holds_alternative<DescAttachment>( resource.desc ) )
				{
					_context->destroy( std::get<DescAttachment>( resource.desc ), resource.id );
				}
				else if ( std::holds_alternative<DescProgram>( resource.desc ) )
				{
					_context->destroy( std::get<DescProgram>( resource.desc ), resource.id );
				}
				else
				{
					throw std::runtime_error( "unknown descriptor type" );
				}
			}

			_resources.clear();
		}
	};

} // namespace VTX::Renderer
#endif
