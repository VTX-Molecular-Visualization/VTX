#include "renderer/render_graph.hpp"
#include "renderer/passes.hpp"

namespace
{

} // namespace

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
			_output = nullptr;
		}

		std::erase_if( _passes, [ &p_pass ]( const std::unique_ptr<Pass> & p_e ) { return p_e.get() == p_pass; } );
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

	RenderGraph::PipelinePasses RenderGraph::createDefaultPipeline( const PipelineConfig & p_config )
	{
		clear();

		PipelinePasses passes;

		// Geometric.
		passes.geo = addPass( descPassGeometric );

		// Depth.
		passes.depth = addPass( descPassDepth );
		addLink( passes.geo, passes.depth, E_CHAN_OUT::DEPTH, E_CHAN_IN::_0 );

		// SSAO + blur.
		if ( p_config.enableSSAO )
		{
			passes.ssao	 = addPass( descPassSSAO );
			passes.blurX = addPass( descPassBlur );
			passes.blurY = addPass( descPassBlur );

			passes.blurX->name = "BlurX";
			passes.blurY->name = "BlurY";

			passes.blurY->programs[ 0 ].data[ 0 ].value = BufferValue<Vec2i> { Vec2i( 0, 1 ) };

			addLink( passes.geo, passes.ssao, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
			addLink( passes.depth, passes.ssao, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_2 );
			addLink( passes.ssao, passes.blurX, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
			addLink( passes.depth, passes.blurX, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_1 );
			addLink( passes.blurX, passes.blurY, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
			addLink( passes.depth, passes.blurY, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_1 );
		}

		// Shading.
		passes.shading = addPass( descPassShading );
		addLink( passes.geo, passes.shading, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
		addLink( passes.geo, passes.shading, E_CHAN_OUT::COLOR_1, E_CHAN_IN::_1 );
		if ( passes.ssao )
		{
			addLink( passes.blurY, passes.shading, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_2 );
		}

		// Outline.
		if ( p_config.enableOutline )
		{
			passes.outline = addPass( descPassOutline );
			addLink( passes.shading, passes.outline, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
			addLink( passes.depth, passes.outline, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_1 );
		}

		// Selection.
		if ( p_config.enableSelection )
		{
			passes.selection = addPass( descPassSelection );
			addLink( passes.geo, passes.selection, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
			addLink( passes.depth, passes.selection, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_2 );

			if ( passes.outline )
			{
				addLink( passes.outline, passes.selection, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_1 );
			}
			else
			{
				addLink( passes.shading, passes.selection, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_1 );
			}
		}

		// FXAA.
		passes.fxaa = addPass( desPassFXAA );
		setOutput( &passes.fxaa->outputs[ E_CHAN_OUT::COLOR_0 ] );

		if ( passes.selection )
		{
			addLink( passes.selection, passes.fxaa, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
		}
		else if ( passes.outline )
		{
			addLink( passes.outline, passes.fxaa, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
		}
		else
		{
			addLink( passes.shading, passes.fxaa, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
		}

		return passes;
	}

} // namespace VTX::Renderer
