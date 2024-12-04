#ifndef __VTX_RENDERER_RENDER_GRAPH__
#define __VTX_RENDERER_RENDER_GRAPH__

#include "context/concept_context.hpp"
#include "scheduler/concept_scheduler.hpp"
#include <util/logger.hpp>
#include <util/variant.hpp>

namespace VTX::Renderer
{
	/**
	 * @brief A graph with nodes (passes) and links.
	 * @tparam C The render context.
	 * @tparam S The scheduler that convert graph to render queue.
	 */
	template<Context::Concept C, Scheduler::Concept S>
	class RenderGraph
	{
	  public:
		RenderGraph() = default;
		~RenderGraph() { clean(); }

		inline Passes &		  getPasses() { return _passes; }
		inline Links &		  getLinks() { return _links; }
		inline RenderQueue &  getRenderQueue() { return _renderQueue; }
		inline const Output * getOutput() { return _output; }
		inline void			  setOutput( const Output * const p_output ) { _output = p_output; }

		inline bool isInRenderQueue( const Pass * const p_pass )
		{
			return std::find( _renderQueue.begin(), _renderQueue.end(), p_pass ) != _renderQueue.end();
		}

		inline Pass * const addPass( const Pass & p_pass )
		{
			_passes.emplace_back( std::make_unique<Pass>( p_pass ) );
			return _passes.back().get();
		}

		void removePass( const Pass * const p_pass )
		{
			// Don't remove geometry pass.
			// TODO: use a flag to set mandatory passes.
			if ( p_pass->name == descPassGeometric.name )
			{
				VTX_ERROR( "Can not remove geometric pass" );
				return;
			}

			std::erase_if(
				_links,
				[ &p_pass ]( const std::unique_ptr<Link> & p_e ) { return p_e->src == p_pass || p_e->dest == p_pass; }
			);

			if ( std::find_if(
					 p_pass->outputs.begin(),
					 p_pass->outputs.end(),
					 [ this ]( const auto & p_element ) { return &p_element.second == _output; }
				 )
				 != p_pass->outputs.end() )
			{
				VTX_DEBUG( "{}", "REMOVE OUTPUT" );
				_output = nullptr;
			}

			std::erase_if( _passes, [ &p_pass ]( const std::unique_ptr<Pass> & p_e ) { return p_e.get() == p_pass; } );
			std::erase( _renderQueue, p_pass );
		}

		bool addLink(
			Pass * const	   p_passSrc,
			Pass * const	   p_passDest,
			const E_CHAN_OUT & p_channelSrc	 = E_CHAN_OUT::COLOR_0,
			const E_CHAN_IN &  p_channelDest = E_CHAN_IN::_0
		)
		{
			// Check pass existence.
			assert( p_passSrc );
			assert( p_passDest );
			//  Check I/O existence.
			assert( p_passSrc->outputs.contains( p_channelSrc ) );
			assert( p_passDest->inputs.contains( p_channelDest ) );

			// Check descriptor compatibility.
			StructCompareVisitorDesc visitor;

			bool areCompatible = std::visit(
				visitor, p_passSrc->outputs[ p_channelSrc ].desc, p_passDest->inputs[ p_channelDest ].desc
			);

			if ( not areCompatible )
			{
				VTX_WARNING( "{}", "Descriptors are not compatible" );
				return false;
			}

			// Remove input if already used.
			std::erase_if(
				_links,
				[ p_passDest, p_channelDest ]( const std::unique_ptr<Link> & p_element )
				{ return p_element.get()->dest == p_passDest && p_element.get()->channelDest == p_channelDest; }
			);

			// Create link.
			_links.emplace_back( std::make_unique<Link>( Link { p_passSrc, p_passDest, p_channelSrc, p_channelDest } )
			);

			return true;
		}

		void removeLink( const Link * const p_link )
		{
			std::erase_if( _links, [ &p_link ]( const std::unique_ptr<Link> & p_e ) { return p_e.get() == p_link; } );
		}

		C * setup(
			void * const				 p_loader,
			const size_t				 p_width,
			const size_t				 p_height,
			const FilePath &			 p_shaderPath,
			Instructions &				 p_outInstructions,
			InstructionsDurationRanges & p_outInstructionsDurationRanges,
			const Handle				 p_output = 0
		)
		{
			VTX_DEBUG( "{}", "Building render graph..." );

			// Check ouptut.
			if ( _output == nullptr )
			{
				VTX_ERROR( "{}", "No output defined" );
				return nullptr;
			}

			// Compute queue with scheduler.
			try
			{
				_scheduler.schedule( _passes, _links, _output, _renderQueue );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Can not build render queue: {}", p_e.what() );
				return nullptr;
			}

			// Some checks.
			if ( _renderQueue.empty() )
			{
				VTX_ERROR( "{}", "Render queue is empty" );
				return nullptr;
			}

			if ( _renderQueue.back()->outputs.size() != 1 )
			{
				// Useless?
				// VTX_ERROR( "{}", "The output of the last pass must be unique" );
				// return nullptr;
			}

			// Create context.
			if ( _context == nullptr )
			{
				_context = std::make_unique<C>( p_width, p_height, p_shaderPath, p_loader );
			}

			// Generate instructions.
			try
			{
				VTX_DEBUG( "{}", "Generating instructions..." );
				_context->build(
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
			return _context.get();
		}

		void clean()
		{
			_renderQueue.clear();
			_context.reset( nullptr );
		}

		inline void addGlobalData( const BufferData & p_globalData ) { _globalData.emplace_back( p_globalData ); }

	  private:
		S				   _scheduler;
		RenderQueue		   _renderQueue;
		std::unique_ptr<C> _context;

		const Output *			_output;
		Passes					_passes;
		std::vector<BufferData> _globalData;
		Links					_links;
	};

} // namespace VTX::Renderer
#endif
