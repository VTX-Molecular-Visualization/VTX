#include "renderer/render_graph.hpp"
#include "renderer/passes.hpp"

namespace VTX::Renderer
{

	Pass * const RenderGraph::addPass( const Pass & p_pass )
	{
		_passes.emplace_back( std::make_unique<Pass>( p_pass ) );
		return _passes.back().get();
	}

	void RenderGraph::removePass( const Pass * const p_pass )
	{
		// Don't remove geometry pass.
		// TODO: use a flag to set mandatory passes?
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

	bool RenderGraph::addLink(
		Pass * const	   p_passSrc,
		Pass * const	   p_passDest,
		const E_CHAN_OUT & p_channelSrc,
		const E_CHAN_IN &  p_channelDest
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

		bool areCompatible
			= std::visit( visitor, p_passSrc->outputs[ p_channelSrc ].desc, p_passDest->inputs[ p_channelDest ].desc );

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
		_links.emplace_back( std::make_unique<Link>( Link { p_passSrc, p_passDest, p_channelSrc, p_channelDest } ) );

		return true;
	}

	void RenderGraph::removeLink( const Link * const p_link )
	{
		std::erase_if( _links, [ &p_link ]( const std::unique_ptr<Link> & p_e ) { return p_e.get() == p_link; } );
	}

} // namespace VTX::Renderer
