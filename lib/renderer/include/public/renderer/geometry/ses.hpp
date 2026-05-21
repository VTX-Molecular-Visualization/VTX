#ifndef __VTX_RENDERER_GEOMETRY_SES__
#define __VTX_RENDERER_GEOMETRY_SES__

#include "base_geometry.hpp"
#include <array>
#include <cstdint>
#include <map>
#include <memory>
#include <numeric>
#include <vector>

namespace VTX::Renderer::Geometry
{
	class SES
	{
	  public:
		using SurfaceID = uint32_t;

		enum class E_SURFACE_SCOPE : uint8_t
		{
			WHOLE,
		};

		struct Surface
		{
			SurfaceID		id	   = 0;
			Desc::Handle	system = Desc::NO_HANDLE;
			E_SURFACE_SCOPE scope  = E_SURFACE_SCOPE::WHOLE;
			uint32_t		index  = 0;
		};

		struct SurfaceKey
		{
			Desc::Handle	system = Desc::NO_HANDLE;
			E_SURFACE_SCOPE scope  = E_SURFACE_SCOPE::WHOLE;
			uint32_t		index  = 0;

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
				nextID = 0;
				ids.clear();
				bySystem.clear();
			}

			SurfaceID									   nextID = 0;
			std::map<SurfaceKey, SurfaceID>				   ids;
			std::map<Desc::Handle, std::vector<SurfaceID>> bySystem;
		};

		class PatchGeometry : public BaseGeometry
		{
		  public:
			void construct(
				const SurfaceID	   p_surface,
				const Desc::Handle p_system,
				const Index		   p_count,
				const uint32_t	   p_dataOffset = 0
			)
			{
				_addRange( p_surface, p_count, p_count );
				_systems.emplace( p_surface, p_system );
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

			void clear()
			{
				BaseGeometry::clear();
				_systems.clear();
				_dataOffsets.clear();
				totalSize = 0;
			}

			void resize( Context::ContextWrapper & p_context )
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

			void uploadIndexes( Context::ContextWrapper & p_context, const SurfaceID p_surface )
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
					_dataOffset( p_surface ) } };
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

			std::map<SurfaceID, Desc::Handle> _systems;
			std::map<SurfaceID, uint32_t>	  _dataOffsets;
		};

		SES();
		~SES();

		static constexpr uint32_t MAX_PROBE_NEIGHBOR_NB = 32u;

		inline static const Desc::Key BUFFER_ATOMS				   = "SES.Atoms";
		inline static const Desc::Key BUFFER_ATOM_IDS			   = "SES.AtomIds";
		inline static const Desc::Key BUFFER_SECTORS			   = "SES.Sectors";
		inline static const Desc::Key BUFFER_PROBES				   = "SES.Probes";
		inline static const Desc::Key BUFFER_PROBE_ATOM_INDICES	   = "SES.ProbesAtomIndices";
		inline static const Desc::Key BUFFER_PROBE_NEIGHBORS	   = "SES.ProbeNeighbors";
		inline static const Desc::Key BUFFER_CONVEX_PATCH_ELEMENTS = "SES.ConvexPatches.Elements";
		inline static const Desc::Key BUFFER_CIRCLE_PATCH_ATOMS	   = "SES.CirclePatches.Atoms";
		inline static const Desc::Key BUFFER_SEGMENT_PATCH_IDS	   = "SES.SegmentPatches.Ids";

		inline static const Desc::Key		  GEOMETRY_CONVEX_PATCHES		   = "SES.ConvexPatches";
		inline static const Desc::Key		  GEOMETRY_CIRCLE_PATCHES		   = "SES.CirclePatches";
		inline static const Desc::Key		  GEOMETRY_SEGMENT_PATCHES		   = "SES.SegmentPatches";
		inline static const Desc::Key		  GEOMETRY_CONCAVE_PATCHES		   = "SES.ConcavePatches";
		inline static const Desc::Key		  INDIRECT_CONVEX_PATCHES		   = "Indirect.SES.ConvexPatches";
		inline static const Desc::Key		  INDIRECT_CIRCLE_PATCHES		   = "Indirect.SES.CirclePatches";
		inline static const Desc::Key		  INDIRECT_SEGMENT_PATCHES		   = "Indirect.SES.SegmentPatches";
		inline static const Desc::Key		  INDIRECT_CONCAVE_PATCHES		   = "Indirect.SES.ConcavePatches";
		inline static constexpr Desc::Binding BINDING_INDIRECT_CONVEX_PATCHES  = 24;
		inline static constexpr Desc::Binding BINDING_INDIRECT_CIRCLE_PATCHES  = 25;
		inline static constexpr Desc::Binding BINDING_INDIRECT_SEGMENT_PATCHES = 26;
		inline static constexpr Desc::Binding BINDING_INDIRECT_CONCAVE_PATCHES = 27;
		inline static const Desc::Key		  INDEX_CONVEX_PATCHES			   = "Index.SES.ConvexPatches";
		inline static const Desc::Key		  INDEX_CIRCLE_PATCHES			   = "Index.SES.CirclePatches";
		inline static const Desc::Key		  INDEX_SEGMENT_PATCHES			   = "Index.SES.SegmentPatches";
		inline static const Desc::Key		  INDEX_CONCAVE_PATCHES			   = "Index.SES.ConcavePatches";
		inline static const Desc::Key		  PASS_COMPUTE					   = "SES.Compute";

		PatchGeometry convexPatches;
		PatchGeometry circlePatches;
		PatchGeometry segmentPatches;
		PatchGeometry concavePatches;

		struct SurfaceConstruction;

		void construct(
			Context::ContextWrapper & p_context,
			Desc::Handle			  p_handle,
			const SystemData &		  p_data,
			uint32_t				  p_inputAtomOffset
		);

		void resize( Context::ContextWrapper & p_context );

		void clear();

		void uploadIndexes( Context::ContextWrapper & p_context, const Desc::Handle p_handle );

		[[nodiscard]] bool built( Desc::Handle p_handle ) const;

		void setVisibility( Desc::Handle p_handle, bool p_visible );

		void compute( Context::ContextWrapper & p_context );

		[[nodiscard]] bool hasPendingCompute() const;

	  protected:
		std::map<SurfaceID, std::unique_ptr<SurfaceConstruction>> _constructions;

	  private:
		Surface _createSurface( const SurfaceKey & );
		Surface _createWholeSurface( Desc::Handle );
		void	_constructEmptyRanges( const Surface & );
		void	_disableDraws( Context::ContextWrapper &, SurfaceID );
		void	_discardPendingCompute( Context::ContextWrapper & );

		SurfaceRegistry _surfaces;
	};

} // namespace VTX::Renderer::Geometry

#endif
