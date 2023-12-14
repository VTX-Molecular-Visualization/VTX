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
		~RenderGraph() { clean(); }

		inline Passes &		  getPasses() { return _passes; }
		inline Links &		  getLinks() { return _links; }
		inline RenderQueue &  getRenderQueue() { return _renderQueue; }
		inline const Output * getOutput() { return _output; }
		inline void			  setOutput( const Output * const p_output ) { _output = p_output; }
		inline bool			  isBuilt() { return _context != nullptr; }

		inline bool isInRenderQueue( const Pass * const p_pass )
		{
			return std::find( _renderQueue.begin(), _renderQueue.end(), p_pass ) != _renderQueue.end();
		}

		inline Pass * const addPass( const Pass & p_pass )
		{
			_passes.emplace_back( std::make_unique<Pass>( p_pass ) );
			return _passes.back().get();
		}

		inline void addUniforms( const Uniforms & p_uniforms )
		{
			_uniforms.insert( _uniforms.end(), p_uniforms.begin(), p_uniforms.end() );
		}

		bool addLink(
			Pass * const			 p_passSrc,
			Pass * const			 p_passDest,
			const E_CHANNEL_OUTPUT & p_channelSrc  = E_CHANNEL_OUTPUT::COLOR_0,
			const E_CHANNEL_INPUT &	 p_channelDest = E_CHANNEL_INPUT::_0
		)
		{
			// Check I/O existence.
			assert( p_passSrc->outputs.contains( p_channelSrc ) );
			assert( p_passDest->inputs.contains( p_channelDest ) );

			// Check descriptor compatibility.
			StructCompareVisitorDesc visitor;

			bool areCompatible = std::visit(
				visitor, p_passSrc->outputs[ p_channelSrc ].desc, p_passDest->inputs[ p_channelDest ].desc
			);

			if ( areCompatible == false )
			{
				VTX_WARNING( "{}", "Descriptors are not compatible" );
				return false;
			}

			// Check input is free.
			if ( std::find_if(
					 _links.begin(),
					 _links.end(),
					 [ p_passDest, p_channelDest ]( const std::unique_ptr<Link> & p_element )
					 { return p_element.get()->dest == p_passDest && p_element.get()->channelDest == p_channelDest; }
				 )
				 != _links.end() )
			{
				VTX_WARNING( "Channel {} from pass {} is already in use", uint( p_channelDest ), p_passDest->name );
				return false;
			}

			// Create link.
			_links.emplace_back( std::make_unique<Link>( Link { p_passSrc, p_passDest, p_channelSrc, p_channelDest } )
			);

			return true;
		}

		void removeLink( const Link * const p_link )
		{
			// TODO: is link deleted?
			std::erase_if( _links, [ &p_link ]( const std::unique_ptr<Link> & p_e ) { return p_e.get() == p_link; } );
		}

		bool setup(
			void * const				 p_loader,
			const size_t				 p_width,
			const size_t				 p_height,
			const FilePath &			 p_shaderPath,
			Instructions &				 p_outInstructions,
			InstructionsDurationRanges & p_outInstructionsDurationRanges,
			const Handle				 p_output = 0
		)
		{
			// Clean all.
			clean();

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
				_scheduler.schedule( _passes, _links, _output, _renderQueue );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Can not build render queue: {}", p_e.what() );
				return false;
			}

			// Some checks.
			if ( _renderQueue.empty() )
			{
				VTX_ERROR( "{}", "Render queue is empty" );
				return false;
			}

			if ( _renderQueue.back()->outputs.size() != 1 )
			{
				VTX_ERROR( "{}", "The output of the last pass must be unique" );
				clean();
				return false;
			}

			// Create context.
			_context = std::make_unique<C>( p_width, p_height, p_shaderPath, p_loader );

			// Generate instructions.
			try
			{
				VTX_DEBUG( "{}", "Generating instructions..." );
				_context->build(
					_renderQueue, _links, p_output, _uniforms, p_outInstructions, p_outInstructionsDurationRanges
				);
				VTX_DEBUG( "{}", "Generating instructions... done" );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Can not generate instructions: {}", p_e.what() );
				p_outInstructions.clear();
				clean();
				return false;
			}

			VTX_DEBUG( "{} instructions generated", p_outInstructions.size() );
			VTX_DEBUG( "{}", "Building render graph... done" );
			return true;
		}

		void resize( const size_t p_width, const size_t p_height )
		{
			assert( _context != nullptr );
			_context->resize( _renderQueue, p_width, p_height );
		}

		void clean()
		{
			_renderQueue.clear();
			_context.reset( nullptr );
		}

		template<typename T>
		inline void setUniform( const T & p_value, const std::string & p_key )
		{
			_context->setUniform( p_value, p_key );
		}

		template<typename T>
		inline void getUniform( T & p_value, const std::string & p_key )
		{
			_context->template getUniform<T>( p_value, p_key );
		}

		template<typename T>
		inline void setData( const std::vector<T> & p_data, const std::string & p_key )
		{
			_context->setData( p_data, p_key );
		}

		inline void fillInfos( StructInfos & p_infos ) { _context->fillInfos( p_infos ); }

		inline float measureTaskDuration( const Util::Chrono::Task & p_task )
		{
			return _context->measureTaskDuration( p_task );
		}

	  private:
		S				   _scheduler;
		RenderQueue		   _renderQueue;
		std::unique_ptr<C> _context;

		const Output * _output;
		Passes		   _passes;
		Uniforms	   _uniforms;
		Links		   _links;
	};

} // namespace VTX::Renderer
#endif
