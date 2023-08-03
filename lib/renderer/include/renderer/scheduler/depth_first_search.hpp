#ifndef __VTX_RENDERER_SCHEDULER_DFS__
#define __VTX_RENDERER_SCHEDULER_DFS__

#include "concept_scheduler.hpp"

namespace VTX::Renderer::Scheduler
{

	class DepthFirstSearch
	{
	  public:
		RenderQueue schedule( Passes & p_passes, const Links & p_links )
		{
			// Build adjacent list.
			std::vector<Pass *>				 passes( p_passes.size() );
			std::vector<std::vector<size_t>> adjacentList( p_passes.size(), std::vector<size_t>() );
			for ( auto & [ name, pass ] : p_passes )
			{
				static size_t i = 0;
				passes[ i++ ]	= &pass;
			}

			for ( const Link & link : p_links )
			{
				adjacentList[ std::distance( passes.begin(), std::find( passes.begin(), passes.end(), link.source ) ) ]
					.push_back(
						std::distance( passes.begin(), std::find( passes.begin(), passes.end(), link.destination ) ) );
			}

			// Topological sort.
			std::vector<bool> visited( passes.size(), false );
			std::vector<bool> onStack( passes.size(), false );

			bool				isCyclic = false;
			std::vector<size_t> sorted;
			for ( size_t index = 0; index < passes.size(); ++index )
			{
				if ( visited[ index ] == false )
				{
					_depthFirstSearch( p_passes, adjacentList, index, visited, onStack, isCyclic, sorted );
					if ( isCyclic )
					{
						throw std::runtime_error( "Cyclic graph" );
					}
				}
			}

			std::reverse( sorted.begin(), sorted.end() );

			return {};
		}

	  private:
		void _depthFirstSearch( Passes &								 p_passes,
								const std::vector<std::vector<size_t>> & p_adjacencyLists,
								const size_t							 p_index,
								std::vector<bool> &						 p_visited,
								std::vector<bool> &						 p_onStack,
								bool &									 p_isCyclic,
								std::vector<size_t> &					 p_sorted )
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

				if ( p_visited[ neighbour ] == false )
				{
					_depthFirstSearch(
						p_passes, p_adjacencyLists, neighbour, p_visited, p_onStack, p_isCyclic, p_sorted );
				}
			}

			p_onStack[ p_index ] = false;
			p_sorted.push_back( p_index );
		}
	};
} // namespace VTX::Renderer::Scheduler

#endif
