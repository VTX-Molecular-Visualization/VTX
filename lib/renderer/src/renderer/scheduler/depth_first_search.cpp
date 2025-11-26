#include "renderer/scheduler/depth_first_search.hpp"
#include <algorithm>
#include <unordered_map>
#include <util/exceptions.hpp>

namespace VTX::Renderer::Scheduler
{

	RenderQueue DepthFirstSearch::schedule( const Passes & p_passes, const Links & p_links, const Output & p_output )
	{
		RenderQueue rq;

		enum struct E_STATE : uint8_t
		{
			NOT_VISITED,
			VISITING,
			VISITED
		};

		std::unordered_map<const Pass *, E_STATE> state;
		state.reserve( p_passes.size() );

		for ( const auto & passPtr : p_passes )
		{
			state[ passPtr.get() ] = E_STATE::NOT_VISITED;
		}

		// Find last pass producing the output.
		const Pass * lastPass = nullptr;
		for ( const auto & passPtr : p_passes )
		{
			const Pass * pass = passPtr.get();
			for ( const auto & [ _, output ] : pass->outputs )
			{
				if ( &output == &p_output )
				{
					lastPass = pass;
					break;
				}
			}
			if ( lastPass != nullptr )
			{
				break;
			}
		}

		if ( lastPass == nullptr )
		{
			throw GraphicException( "No pass produces the output." );
		}

		// Depth-first search function.
		std::function<void( const Pass * )> dfs = [ & ]( const Pass * pass )
		{
			auto it = state.find( pass );
			assert( it != state.end() );

			switch ( it->second )
			{
			case E_STATE::VISITED: return;

			case E_STATE::VISITING: throw GraphicException( "Render graph contains a cycle." );

			case E_STATE::NOT_VISITED: break;
			}

			it->second = E_STATE::VISITING;

			// Visit all predecessor passes.
			for ( const auto & linkPtr : p_links )
			{
				if ( linkPtr->dest == pass )
				{
					dfs( linkPtr->src );
				}
			}

			// Add the current pass to the render queue.
			it->second = E_STATE::VISITED;
			rq.push_back( const_cast<Pass *>( pass ) );
		};

		// Start DFS from the last pass.
		dfs( lastPass );

		return rq;
	}

} // namespace VTX::Renderer::Scheduler
