#include "renderer/scheduler/custom_search.hpp"
#include <util/exceptions.hpp>

namespace VTX::Renderer::Scheduler
{

	void CustomSearch::schedule(
		Passes &			 p_passes,
		const Links &		 p_links,
		const Output * const p_output,
		RenderQueue &		 p_outRenderQueue
	)
	{
		// Build reversed adjacent list.
		std::vector<Pass *>						passes( p_passes.size() );
		std::vector<std::unordered_set<size_t>> adjacentList( p_passes.size(), std::unordered_set<size_t>() );
		size_t									i			  = 0;
		size_t									lastPassIndex = -1;

		for ( auto & pass : p_passes )
		{
			passes[ i ] = pass.get();
			for ( auto & output : pass->outputs )
			{
				if ( &output.second == p_output )
				{
					lastPassIndex = i;
				}
			}
			i++;
		}

		for ( auto & link : p_links )
		{
			adjacentList[ std::distance( passes.begin(), std::find( passes.begin(), passes.end(), link->dest ) ) ]
				.insert( std::distance( passes.begin(), std::find( passes.begin(), passes.end(), link->src ) ) );
		}

		// Topological sort.
		assert( lastPassIndex != -1 );
		p_outRenderQueue.push_back( passes[ lastPassIndex ] );

		_sort( lastPassIndex, passes, adjacentList, p_outRenderQueue );
		std::reverse( p_outRenderQueue.begin(), p_outRenderQueue.end() );
	}

	void CustomSearch::_sort(
		const size_t							  p_currentPass,
		std::vector<Pass *> &					  p_passes,
		std::vector<std::unordered_set<size_t>> & p_adjacentList,
		RenderQueue &							  p_outRenderQueue
	)
	{
		for ( size_t index : p_adjacentList[ p_currentPass ] )
		{
			if ( std::find( p_outRenderQueue.begin(), p_outRenderQueue.end(), p_passes[ index ] )
				 == p_outRenderQueue.end() )
			{
				p_outRenderQueue.push_back( p_passes[ index ] );
				_sort( index, p_passes, p_adjacentList, p_outRenderQueue );
			}
			else
			{
				throw GraphicException( "cyclic graph" );
			}
		}
	}

} // namespace VTX::Renderer::Scheduler
