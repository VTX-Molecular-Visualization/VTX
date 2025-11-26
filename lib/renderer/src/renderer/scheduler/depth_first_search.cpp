#include "renderer/scheduler/depth_first_search.hpp"
#include <algorithm>
#include <unordered_map>
#include <util/exceptions.hpp>

namespace VTX::Renderer::Scheduler
{

	RenderQueue DepthFirstSearch::schedule( const Passes & p_passes, const Links & p_links, const Output & p_output )
	{
		RenderQueue rq;

		// Map to track visited passes.
		std::unordered_map<const Pass *, bool> visited;
		for ( const auto & passPtr : p_passes )
		{
			visited[ passPtr.get() ] = false;
		}

		// Find last pass producing the output.
		const Pass * lastPass = nullptr;
		for ( const auto & linkPtr : p_links )
		{
			if ( linkPtr->dest == nullptr )
			{
				continue;
			}
			for ( const auto & [ _, output ] : linkPtr->dest->outputs )
			{
				if ( &output == &p_output )
				{
					lastPass = linkPtr->src;
					break;
				}
			}
		}

		if ( lastPass == nullptr )
		{
			throw GraphicException( "No pass produces the output." );
		}

		// Depth-first search function.
		std::function<void( const Pass * )> dfs = [ & ]( const Pass * pass )
		{
			if ( visited[ pass ] )
			{
				return;
			}
			visited[ pass ] = true;
			// Visit all predecessor passes.
			for ( const auto & linkPtr : p_links )
			{
				if ( linkPtr->dest == pass )
				{
					dfs( linkPtr->src );
				}
			}
			// Add the current pass to the render queue.
			rq.push_back( const_cast<Pass *>( pass ) );
		};

		// Start DFS from the last pass.
		dfs( lastPass );

		return rq;
	}

} // namespace VTX::Renderer::Scheduler
