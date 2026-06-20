#include "renderer/render_graph.hpp"
#include <string>
#include <unordered_set>
#include <util/exceptions.hpp>
#include <util/logger.hpp>
#include <utility>
#include <vector>

namespace VTX::Renderer
{
	bool RenderGraph::setGeometryChunks( const Desc::Key & p_geometry, std::span<const Desc::BufferChunk> p_chunks )
	{
		const auto it = _resources.geometries.find( p_geometry );
		assert( it != _resources.geometries.end() );

		std::vector<Desc::BufferChunk> chunks( p_chunks.begin(), p_chunks.end() );
		if ( it->second.chunks == chunks )
		{
			return false;
		}

		it->second.chunks = std::move( chunks );

		return true;
	}

	const Desc::RenderQueue RenderGraph::build()
	{
		using namespace Desc;

		// Check if all inputs are produced before use.
		std::unordered_set<Key> produced;

		auto isExternalResource = [ this ]( const Key & name ) -> bool
		{
			if ( const auto texture = _resources.textures.find( name );
				 texture != _resources.textures.end() && texture->second.external )
			{
				return true;
			}
			if ( _resources.geometries.contains( name ) )
			{
				return true;
			}
			if ( _resources.buffers.contains( name ) )
			{
				return true;
			}
			return false;
		};

		auto hasInputData = [ this ]( const Key & name ) -> bool
		{
			if ( _resources.textures.contains( name ) )
			{
				const auto it = _resources.textures.find( name );
				assert( it != _resources.textures.end() );
				return not it->second.data.empty();
			}
			return false;
		};

		auto checkBinding = [ this ]( const ResourceBinding & res ) -> bool
		{
			if ( _resources.textures.contains( res.primary ) )
			{
				if ( res.secondary && not _resources.samplers.contains( *res.secondary ) )
				{
					return false;
				}
			}

			return true;
		};

		for ( const auto & passPtr : _passes )
		{
			const Pass & pass = *passPtr;

			for ( const auto & input : pass.inputs )
			{
				if ( not checkBinding( input ) )
				{
					throw GraphicException( "Texture '{}': incorrect binding", input.primary );
				}

				const bool external		  = isExternalResource( input.primary );
				const bool producedBefore = produced.contains( input.primary );
				const bool hasData		  = hasInputData( input.primary );

				// If not external, not produced before and has no data.
				if ( not external && not producedBefore && not hasData )
				{
					throw GraphicException( "Pass '{}' need not produced ressource '{}' ", pass.name, input.primary );
				}
			}

			// Set outputs as produced.
			for ( const auto & output : pass.outputs )
			{
				if ( not checkBinding( output ) )
				{
					throw GraphicException( "Texture '{}': incorrect binding", output.primary );
				}

				produced.insert( output.primary );
			}

			for ( const Program & program : pass.programs )
			{
				if ( program.dispatchIndirect
					 && not _resources.buffers.contains( program.dispatchIndirect->indirectBuffer ) )
				{
					throw GraphicException(
						"Program '{}': dispatch indirect buffer '{}' not found",
						program.name,
						program.dispatchIndirect->indirectBuffer
					);
				}
			}
		}

		// Check geometry resources.
		for ( const auto & [ key, geometry ] : _resources.geometries )
		{
			if ( not _resources.vertexStreams.contains( geometry.vertexLayout ) )
			{
				throw GraphicException( "Geometry '{}': vertex layout '{}' not found", key, geometry.vertexLayout );
			}
			if ( geometry.indiceBuffer && not _resources.buffers.contains( *geometry.indiceBuffer ) )
			{
				throw GraphicException( "Geometry '{}': index buffer '{}' not found", key, *geometry.indiceBuffer );
			}
			if ( geometry.indirectBuffer && not _resources.buffers.contains( *geometry.indirectBuffer ) )
			{
				throw GraphicException(
					"Geometry '{}': indirect buffer '{}' not found", key, *geometry.indirectBuffer
				);
			}
		}

		// Build.
		// TODO: remove unused passes.
		Desc::RenderQueue queue;
		for ( auto & p : _passes )
		{
			queue.push_back( p.get() );
		}

		// Check empty.
		if ( queue.empty() )
		{
			return queue;
		}

		// Check last pass = no output (going to fbo).
		if ( queue.back()->outputs.size() > 0 )
		{
			// throw GraphicException( "Last pass '{}' must have exactly one output", queue.back()->name );
		}

		// Print.
		std::string str = "Passes: ";
		for ( const Pass * const pass : queue )
		{
			str += pass->name + " -> ";
		}
		str += "Output";
		VTX_DEBUG( "{}", str );

		return queue;
	}

	void RenderGraph::add( const GraphBuilder & p_builder )
	{
		// Resources.
		for ( const auto & [ key, texture ] : p_builder.resources.textures )
		{
			auto [ it, inserted ] = _resources.textures.emplace( key, texture );
			assert( inserted );
		}
		for ( const auto & [ key, vertexStream ] : p_builder.resources.vertexStreams )
		{
			auto [ it, inserted ] = _resources.vertexStreams.emplace( key, vertexStream );
			assert( inserted );
		}
		for ( const auto & [ key, buffer ] : p_builder.resources.buffers )
		{
			auto [ it, inserted ] = _resources.buffers.emplace( key, buffer );
			assert( inserted );
		}
		for ( const auto & [ key, sampler ] : p_builder.resources.samplers )
		{
			auto [ it, inserted ] = _resources.samplers.emplace( key, sampler );
		}
		for ( const auto & [ key, geometry ] : p_builder.resources.geometries )
		{
			auto [ it, inserted ] = _resources.geometries.emplace( key, geometry );
			assert( inserted );
		}
		// Passes.
		for ( const auto & pass : p_builder.passes )
		{
			_passes.push_back( std::make_unique<Desc::Pass>( *pass ) );
		}
	}

	void RenderGraph::set( GraphBuilder && p_builder )
	{
		clear();
		_resources = std::move( p_builder.resources );
		_passes	   = std::move( p_builder.passes );
	}

	void RenderGraph::clear()
	{
		// Clear resources?
		_resources = Desc::Resources {};
		_passes.clear();
	}

} // namespace VTX::Renderer
