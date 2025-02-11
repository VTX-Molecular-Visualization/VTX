#include "renderer/scheduler/depth_first_search.hpp"
#include <unordered_set>
#include <util/exceptions.hpp>

namespace VTX::Renderer::Scheduler
{

	void DepthFirstSearch::schedule(
		Passes &			 p_passes,
		const Links &		 p_links,
		const Output * const p_output,
		RenderQueue &		 p_outRenderQueue
	)
	{
		// TODO: fix passes added but not linked to output.
		//
		// Build adjacent list.
		std::vector<Pass *>				 passes( p_passes.size() );
		std::vector<std::vector<size_t>> adjacentList( p_passes.size(), std::vector<size_t>() );
		size_t							 i = 0;
		for ( auto & pass : p_passes )
		{
			passes[ i++ ] = pass.get();
		}

		for ( auto & link : p_links )
		{
			adjacentList[ std::distance( passes.begin(), std::find( passes.begin(), passes.end(), link->src ) ) ]
				.push_back( std::distance( passes.begin(), std::find( passes.begin(), passes.end(), link->dest ) ) );
		}

		// Topological sort.
		std::vector<bool> visited( passes.size(), false );
		std::vector<bool> onStack( passes.size(), false );

		// Disable items not linked to the graph.
		for ( size_t index = 0; index < passes.size(); ++index )
		{
			if ( adjacentList[ index ].size() == 0 )
			{
				// TODO: understand why this is not working.
				// Check output connection.
				/*
				const Outputs & outputs = passes[ index ]->outputs;
				const auto		it		= std::find_if( outputs.begin(),
												outputs.end(),
												[ p_output ]( const std::pair<E_CHANNEL_OUTPUT, Output> & p_e )
												{ return &( p_e.second ) == p_output; } );

				if ( it == outputs.end() )
				{
					visited[ index ] = true;
				}
				*/

				visited[ index ] = true;
				for ( auto & [ channel, output ] : passes[ index ]->outputs )
				{
					if ( &output == p_output )
					{
						visited[ index ] = false;
						break;
					}
				}
			}
		}

		// DFS.
		bool				isCyclic = false;
		std::vector<size_t> sorted;
		for ( size_t index = 0; index < passes.size(); ++index )
		{
			if ( not visited[ index ] )
			{
				_depthFirstSearch( p_passes, adjacentList, index, visited, onStack, isCyclic, sorted );
				if ( isCyclic )
				{
					throw GraphicException( "Cyclic graph" );
				}
			}
		}

		// Render queue.
		std::reverse( sorted.begin(), sorted.end() );
		p_outRenderQueue.resize( sorted.size() );
		for ( size_t index = 0; index < sorted.size(); ++index )
		{
			p_outRenderQueue[ index ] = passes[ sorted[ index ] ];
		}
	}

	void DepthFirstSearch::_depthFirstSearch(
		Passes &								 p_passes,
		const std::vector<std::vector<size_t>> & p_adjacencyLists,
		const size_t							 p_index,
		std::vector<bool> &						 p_visited,
		std::vector<bool> &						 p_onStack,
		bool &									 p_isCyclic,
		std::vector<size_t> &					 p_sorted
	)
	{
		if ( p_isCyclic )
		{
			return;
		}

		p_visited[ p_index ] = true;
		p_onStack[ p_index ] = true;

		for ( size_t neighbour : p_adjacencyLists[ p_index ] )
		{
			if ( p_visited[ neighbour ] && p_onStack[ neighbour ] )
			{
				p_isCyclic = true;
				return;
			}

			if ( not p_visited[ neighbour ] )
			{
				_depthFirstSearch( p_passes, p_adjacencyLists, neighbour, p_visited, p_onStack, p_isCyclic, p_sorted );
			}
		}

		p_onStack[ p_index ] = false;
		p_sorted.push_back( p_index );
	}

} // namespace VTX::Renderer::Scheduler
