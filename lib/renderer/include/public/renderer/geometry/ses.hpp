#ifndef __VTX_RENDERER_GEOMETRY_SES__
#define __VTX_RENDERER_GEOMETRY_SES__

#include "base_geometry.hpp"
#include "renderer/caches.hpp"
#include "renderer/representation.hpp"
#include "util/math/bitset.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <map>
#include <memory>
#include <numeric>
#include <span>
#include <string_view>
#include <utility>
#include <vector>

namespace VTX::Renderer::Geometry
{
	class SES
	{
	  public:
		using SurfaceID = uint32_t;

		struct Surface
		{
			SurfaceID		   id	  = 0;
			Desc::Handle	   system = Desc::NO_HANDLE;
			E_SES_COMPUTE_MODE scope  = E_SES_COMPUTE_MODE::NONE;
			uint32_t		   index  = 0;
		};

		struct SurfaceKey
		{
			Desc::Handle	   system = Desc::NO_HANDLE;
			E_SES_COMPUTE_MODE scope  = E_SES_COMPUTE_MODE::NONE;
			uint32_t		   index  = 0;

			[[nodiscard]] bool operator<( const SurfaceKey & p_other ) const
			{
				if ( system != p_other.system )
				{
					return system < p_other.system;
				}

				if ( scope != p_other.scope )
				{
					return scope < p_other.scope;
				}

				return index < p_other.index;
			}
		};

		struct SurfaceRegistry
		{
			void clear()
			{
				nextID = 1;
				ids.clear();
				bySystem.clear();
			}

			SurfaceID									   nextID = 1;
			std::map<SurfaceKey, SurfaceID>				   ids;
			std::map<Desc::Handle, std::vector<SurfaceID>> bySystem;
		};

		class PatchGeometry : public BaseGeometry
		{
		  public:
			void construct(
				const SurfaceID			  p_surface,
				const Desc::Handle		  p_system,
				const Index				  p_count,
				const RepresentationIndex p_representation,
				const uint32_t			  p_dataOffset = 0
			)
			{
				if ( contains( p_surface ) )
				{
					remove( p_surface );
				}

				_addRange( p_surface, p_count, p_count );
				_systems.emplace( p_surface, p_system );
				_representations.emplace( p_surface, p_representation );
				_dataOffsets.emplace( p_surface, p_dataOffset );
				totalSize += p_count;

				if ( p_count > 0 )
				{
					chunks.emplace_back( p_surface );
				}

				auto & indices = _indices( p_surface );
				indices.resize( p_count );
				std::iota( indices.begin(), indices.end(), 0 );
			}

			void clear() override
			{
				BaseGeometry::clear();
				_systems.clear();
				_representations.clear();
				_dataOffsets.clear();
				totalSize = 0;
			}

			void remove( const SurfaceID p_surface )
			{
				const auto it = _data().find( p_surface );
				if ( it != _data().end() )
				{
					totalSize -= it->second.range.getCount();
				}

				_removeRange( p_surface );
				_systems.erase( p_surface );
				_representations.erase( p_surface );
				_dataOffsets.erase( p_surface );
				std::erase( chunks, p_surface );
			}

			[[nodiscard]] bool contains( const SurfaceID p_surface ) const { return _hasRange( p_surface ); }

			void resize( Context::ContextWrapper & p_context ) override
			{
				assert( indiceBuffer );

				for ( const auto & [ surface, data ] : _data() )
				{
					if ( data.indices.empty() )
					{
						continue;
					}

					const Desc::BufferRef ref { *indiceBuffer, surface };
					p_context.ensureBufferChunk( ref );
					p_context.setBuffer<Indice>( ref, data.indices.size() );
				}
			}

			void uploadIndexes( Context::ContextWrapper & p_context, const SurfaceID p_surface ) override
			{
				assert( indiceBuffer );

				const auto it = _data().find( p_surface );
				assert( it != _data().end() );
				const auto & data = it->second;
				if ( data.indices.empty() )
				{
					return;
				}

				const Desc::BufferRef ref { *indiceBuffer, p_surface };
				p_context.ensureBufferChunk( ref );
				p_context.setBuffer<Indice>( ref, data.indices );
			}

			void setVisibility( const SurfaceID p_surface, const bool p_visible )
			{
				const auto it = _data().find( p_surface );
				assert( it != _data().end() );

				auto & indices = _indices( p_surface );
				indices.clear();

				if ( not p_visible )
				{
					return;
				}

				indices.resize( it->second.range.getCount() );
				std::iota( indices.begin(), indices.end(), 0 );
			}

			void setIndices( const SurfaceID p_surface, std::vector<Indice> p_indices )
			{
				assert( _data().contains( p_surface ) );
				_indices( p_surface ) = std::move( p_indices );
			}

			[[nodiscard]] std::vector<Desc::DrawIndexedIndirectRecord> toDrawIndexedIndirectCommands(
				const SurfaceID p_surface
			) const
			{
				const auto it = _data().find( p_surface );
				assert( it != _data().end() );
				const auto & data = it->second;
				if ( data.indices.empty() )
				{
					return {};
				}

				return { Desc::DrawIndexedIndirectRecord {
					Desc::DrawIndexedIndirectCommand { static_cast<uint32_t>( data.indices.size() ), 1, 0, 0, 0 },
					static_cast<uint32_t>( _system( p_surface ) ),
					_dataOffset( p_surface ),
					static_cast<uint32_t>( _representation( p_surface ) ) } };
			}

			Index totalSize = 0;

		  private:
			Desc::Handle _system( const SurfaceID p_surface ) const
			{
				const auto it = _systems.find( p_surface );
				assert( it != _systems.end() );
				return it->second;
			}

			uint32_t _dataOffset( const SurfaceID p_surface ) const
			{
				const auto it = _dataOffsets.find( p_surface );
				assert( it != _dataOffsets.end() );
				return it->second;
			}

			RepresentationIndex _representation( const SurfaceID p_surface ) const
			{
				const auto it = _representations.find( p_surface );
				assert( it != _representations.end() );
				return it->second;
			}

			std::map<SurfaceID, Desc::Handle>		 _systems;
			std::map<SurfaceID, RepresentationIndex> _representations;
			std::map<SurfaceID, uint32_t>			 _dataOffsets;
		};

		SES();
		~SES();

		static constexpr uint32_t MAX_PROBE_NEIGHBOR_NB = 32u;

		static constexpr std::string_view BUFFER_ATOMS				   = "SES.Atoms";
		static constexpr std::string_view BUFFER_ATOM_IDS			   = "SES.AtomIds";
		static constexpr std::string_view BUFFER_SECTORS			   = "SES.Sectors";
		static constexpr std::string_view BUFFER_PROBES				   = "SES.Probes";
		static constexpr std::string_view BUFFER_PROBE_ATOM_INDICES	   = "SES.ProbesAtomIndices";
		static constexpr std::string_view BUFFER_PROBE_NEIGHBORS	   = "SES.ProbeNeighbors";
		static constexpr std::string_view BUFFER_CONVEX_PATCH_ELEMENTS = "SES.ConvexPatches.Elements";
		static constexpr std::string_view BUFFER_CIRCLE_PATCH_ATOMS	   = "SES.CirclePatches.Atoms";
		static constexpr std::string_view BUFFER_SEGMENT_PATCH_IDS	   = "SES.SegmentPatches.Ids";

		static constexpr std::string_view GEOMETRY_CONVEX_PATCHES		   = "SES.ConvexPatches";
		static constexpr std::string_view GEOMETRY_CIRCLE_PATCHES		   = "SES.CirclePatches";
		static constexpr std::string_view GEOMETRY_SEGMENT_PATCHES		   = "SES.SegmentPatches";
		static constexpr std::string_view GEOMETRY_CONCAVE_PATCHES		   = "SES.ConcavePatches";
		static constexpr std::string_view INDIRECT_CONVEX_PATCHES		   = "Indirect.SES.ConvexPatches";
		static constexpr std::string_view INDIRECT_CIRCLE_PATCHES		   = "Indirect.SES.CirclePatches";
		static constexpr std::string_view INDIRECT_SEGMENT_PATCHES		   = "Indirect.SES.SegmentPatches";
		static constexpr std::string_view INDIRECT_CONCAVE_PATCHES		   = "Indirect.SES.ConcavePatches";
		static constexpr Desc::Binding	  BINDING_INDIRECT_CONVEX_PATCHES  = 10;
		static constexpr Desc::Binding	  BINDING_INDIRECT_CIRCLE_PATCHES  = 10;
		static constexpr Desc::Binding	  BINDING_INDIRECT_SEGMENT_PATCHES = 10;
		static constexpr Desc::Binding	  BINDING_INDIRECT_CONCAVE_PATCHES = 10;
		static constexpr std::string_view INDEX_CONVEX_PATCHES			   = "Index.SES.ConvexPatches";
		static constexpr std::string_view INDEX_CIRCLE_PATCHES			   = "Index.SES.CirclePatches";
		static constexpr std::string_view INDEX_SEGMENT_PATCHES			   = "Index.SES.SegmentPatches";
		static constexpr std::string_view INDEX_CONCAVE_PATCHES			   = "Index.SES.ConcavePatches";
		static constexpr std::string_view PASS_COMPUTE					   = "SES.Compute";

		PatchGeometry convexPatches;
		PatchGeometry circlePatches;
		PatchGeometry segmentPatches;
		PatchGeometry concavePatches;

		struct SurfaceConstruction;

		void construct(
			Context::ContextWrapper & p_context,
			Desc::Handle			  p_handle,
			const Cache::System &	  p_data,
			uint32_t				  p_inputAtomOffset,
			float					  p_probeRadius,
			E_SES_COMPUTE_MODE		  p_computeMode,
			RepresentationIndex		  p_representation
		);

		void resize( Context::ContextWrapper & p_context );

		void clear();
		void remove( Context::ContextWrapper & p_context, Desc::Handle p_handle );
		void invalidate( Desc::Handle p_handle );
		void invalidateForRecompute( Context::ContextWrapper & p_context, Desc::Handle p_handle );

		void uploadIndexes( Context::ContextWrapper & p_context, Desc::Handle p_handle );

		[[nodiscard]] bool				 built( Desc::Handle p_handle ) const;
		[[nodiscard]] float				 probeRadius( Desc::Handle p_handle ) const;
		[[nodiscard]] E_SES_COMPUTE_MODE computeMode( Desc::Handle p_handle ) const;

		void setVisibility( Desc::Handle p_handle, bool p_visible );
		void setVisibility( Desc::Handle p_handle, const Util::Math::BitSet & p_visibility );

		void compute( Context::ContextWrapper & p_context );

		[[nodiscard]] bool hasPendingCompute() const;

	  protected:
		std::map<SurfaceID, std::unique_ptr<SurfaceConstruction>> _constructions;

	  private:
		Surface _createSurface( const SurfaceKey & );
		Surface _getOrCreateSurface( const SurfaceKey & );
		void	_constructSurface(
			Context::ContextWrapper & p_context,
			const Cache::System &	  p_data,
			uint32_t				  p_inputAtomOffset,
			float					  p_probeRadius,
			E_SES_COMPUTE_MODE		  p_computeMode,
			RepresentationIndex		  p_representation,
			const Surface &			  p_surface,
			std::span<const Index>	  p_atomIndices
		);
		static void _releaseChunks( Context::ContextWrapper &, SurfaceID );
		static void _unregisterCudaInputSourceBuffers( Context::ContextWrapper & );
		static void _unregisterCudaConstructionBuffers( Context::ContextWrapper &, SurfaceID );
		static void _unregisterCudaSurfaceBuffers( Context::ContextWrapper &, SurfaceID );
		void		_constructEmptyRanges( const Surface & );
		void		_disableDraws( Context::ContextWrapper &, SurfaceID ) const;

		SurfaceRegistry _surfaces;
	};

} // namespace VTX::Renderer::Geometry

#endif
