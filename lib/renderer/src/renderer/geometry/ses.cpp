#include "renderer/geometry/ses.hpp"
#include "renderer/binary_buffer.hpp"
#include "renderer/layout/atoms.hpp"
#include "renderer/representation.hpp"
#include <algorithm>
#include <array>
#include <utility>
#include <util/chrono.hpp>
#include <util/logger.hpp>
#include <vector>
#ifdef VTX_CUDA_ENABLED
#include "renderer/geometry/ses_cuda.hpp"
#endif

namespace VTX::Renderer::Geometry
{
	struct SES::SurfaceConstruction
	{
		enum class State : uint8_t
		{
			Empty,
			PendingWrite,
			Written,
			Incalculable,
		};

#ifdef VTX_CUDA_ENABLED
		SESDetail::CudaConstructionPtr cudaConstruction;
#endif

		struct VisibilityData
		{
			std::vector<uint32_t>				   atomIds;
			std::vector<std::array<uint32_t, 2>> circleAtoms;
			std::vector<std::array<uint32_t, 2>> segmentAtoms;
			std::vector<std::array<int32_t, 4>>  concaveAtoms;
		};

		Surface			   surface;
		State			   state			  = State::Empty;
		uint32_t		   atomOffset		  = 0;
		uint32_t		   rendererAtomOffset = 0;
		uint32_t		   atomNb			  = 0;
		uint32_t		   probeOffset		  = 0;
		uint32_t		   sectorOffset		  = 0;
		uint32_t		   convexPatchNb	  = 0;
		uint32_t		   circlePatchNb	  = 0;
		uint32_t		   segmentPatchNb	  = 0;
		uint32_t		   probeNb			  = 0;
		uint32_t		   sectorNb			  = 0;
		uint32_t		   concavePatchNb	  = 0;
		float			   probeRadius		  = SES_PROBE_RADIUS_DEFAULT;
		E_SES_COMPUTE_MODE computeMode		  = SES_COMPUTE_MODE_DEFAULT;
		VisibilityData	   visibility;

		[[nodiscard]] bool pendingWrite() const
		{
#ifdef VTX_CUDA_ENABLED
			return state == State::PendingWrite && cudaConstruction != nullptr;
#else
			return false;
#endif
		}

		void markIncalculable()
		{
#ifdef VTX_CUDA_ENABLED
			cudaConstruction.reset();
#endif
			state = State::Incalculable;
		}
	};

	namespace
	{
		BinaryBuffer430 _emptyIndirectBuffer()
		{
			BinaryBuffer430 buffer;
			buffer.write( uint32_t( 0 ) );
			buffer.alignTo( Desc::DRAW_INDIRECT_COMMANDS_OFFSET );
			buffer.close();
			return buffer;
		}

		bool _isSesCategoryAllowed( const Core::ChemDB::Category::TYPE p_category )
		{
			return p_category == Core::ChemDB::Category::TYPE::POLYMER
				   || p_category == Core::ChemDB::Category::TYPE::CARBOHYDRATE
				   || p_category == Core::ChemDB::Category::TYPE::LIGAND;
		}

#ifdef VTX_CUDA_ENABLED
		constexpr size_t OUTPUT_BUFFER_NB = 7;

		std::vector<Desc::BufferRef> _outputBufferRefs( const SES::SurfaceConstruction & p_construction )
		{
			const uint32_t				 chunk = p_construction.surface.id;
			std::vector<Desc::BufferRef> refs;

			if ( p_construction.convexPatchNb > 0 )
			{
				refs.emplace_back( SES::BUFFER_CONVEX_PATCH_ELEMENTS, chunk );
			}
			if ( p_construction.circlePatchNb > 0 )
			{
				refs.emplace_back( SES::BUFFER_CIRCLE_PATCH_ATOMS, chunk );
			}
			if ( p_construction.segmentPatchNb > 0 )
			{
				refs.emplace_back( SES::BUFFER_SEGMENT_PATCH_IDS, chunk );
			}
			if ( p_construction.probeNb > 0 )
			{
				refs.emplace_back( SES::BUFFER_PROBES, chunk );
				refs.emplace_back( SES::BUFFER_PROBE_ATOM_INDICES, chunk );
				refs.emplace_back( SES::BUFFER_PROBE_NEIGHBORS, chunk );
			}
			if ( p_construction.sectorNb > 0 )
			{
				refs.emplace_back( SES::BUFFER_SECTORS, chunk );
			}

			return refs;
		}

		SESDetail::SesdfOutputBuffers _outputBuffers(
			std::span<const Desc::InteropBufferMapping> p_mappings,
			const SES::SurfaceConstruction &			p_construction
		)
		{
			size_t	   mappingIndex = 0;
			const auto nextBuffer	= [ & ]()
			{
				const Desc::InteropBufferMapping & mapping = p_mappings[ mappingIndex++ ];
				return SESDetail::CudaBufferView { mapping.devicePtr, mapping.size, 0 };
			};

			SESDetail::SesdfOutputBuffers outputBuffers;
			if ( p_construction.convexPatchNb > 0 )
			{
				outputBuffers.convexPatches = nextBuffer();
			}
			if ( p_construction.circlePatchNb > 0 )
			{
				outputBuffers.circlePatches = nextBuffer();
			}
			if ( p_construction.segmentPatchNb > 0 )
			{
				outputBuffers.segmentPatches = nextBuffer();
			}
			if ( p_construction.probeNb > 0 )
			{
				outputBuffers.probes		   = nextBuffer();
				outputBuffers.probeAtomIndices = nextBuffer();
				outputBuffers.probeNeighbors   = nextBuffer();
			}
			if ( p_construction.sectorNb > 0 )
			{
				outputBuffers.sectors = nextBuffer();
			}
			assert( mappingIndex == p_mappings.size() );
			outputBuffers.atomIndexOffset		  = p_construction.atomOffset;
			outputBuffers.rendererAtomIndexOffset = p_construction.rendererAtomOffset;
			outputBuffers.probeIndexOffset		  = 0;
			outputBuffers.sectorIndexOffset		  = 0;
			outputBuffers.maxProbeNeighborNb	  = SES::MAX_PROBE_NEIGHBOR_NB;

			return outputBuffers;
		}
#endif

		bool _isSortedAtomVisible(
			const SES::SurfaceConstruction & p_construction,
			const uint32_t					 p_sortedAtom,
			const Util::Math::BitSet &		 p_visibility
		)
		{
			if ( p_sortedAtom >= p_construction.visibility.atomIds.size() )
			{
				return false;
			}

			const uint32_t rendererAtom = p_construction.visibility.atomIds[ p_sortedAtom ];
			if ( rendererAtom < p_construction.rendererAtomOffset )
			{
				return false;
			}

			const uint32_t atom = rendererAtom - p_construction.rendererAtomOffset;
			return atom < p_visibility.size() && p_visibility.test( atom );
		}

		template<typename Predicate>
		std::vector<Indice> _filterPatchIndices( const uint32_t p_count, Predicate && p_predicate )
		{
			std::vector<Indice> indices;
			indices.reserve( p_count );

			for ( uint32_t i = 0; i < p_count; ++i )
			{
				if ( p_predicate( i ) )
				{
					indices.emplace_back( i );
				}
			}

			return indices;
		}
	} // namespace

	SES::SES()
	{
		convexPatches.vertexLayout	 = GEOMETRY_CONVEX_PATCHES;
		convexPatches.indiceBuffer	 = INDEX_CONVEX_PATCHES;
		convexPatches.indirectBuffer = INDIRECT_CONVEX_PATCHES;

		circlePatches.vertexLayout	 = GEOMETRY_CIRCLE_PATCHES;
		circlePatches.indiceBuffer	 = INDEX_CIRCLE_PATCHES;
		circlePatches.indirectBuffer = INDIRECT_CIRCLE_PATCHES;

		segmentPatches.vertexLayout	  = GEOMETRY_SEGMENT_PATCHES;
		segmentPatches.indiceBuffer	  = INDEX_SEGMENT_PATCHES;
		segmentPatches.indirectBuffer = INDIRECT_SEGMENT_PATCHES;

		concavePatches.vertexLayout	  = GEOMETRY_CONCAVE_PATCHES;
		concavePatches.indiceBuffer	  = INDEX_CONCAVE_PATCHES;
		concavePatches.indirectBuffer = INDIRECT_CONCAVE_PATCHES;
	}

	SES::~SES() = default;

	void SES::construct(
		Context::ContextWrapper & p_context,
		const Desc::Handle		  p_handle,
		const Cache::System &	  p_data,
		const uint32_t			  p_inputAtomOffset,
		const float				  p_probeRadius,
		const E_SES_COMPUTE_MODE  p_computeMode,
		const RepresentationIndex p_representation
	)
	{
		if ( p_computeMode == E_SES_COMPUTE_MODE::SYSTEM )
		{
			_constructSurface(
				p_context,
				p_data,
				p_inputAtomOffset,
				p_probeRadius,
				p_computeMode,
				p_representation,
				_getOrCreateWholeSurface( p_handle ),
				{}
			);
			return;
		}

		std::array<std::vector<Index>, size_t( Core::ChemDB::Category::TYPE::COUNT )> atomsByCategory;
		const Core::Struct::Topology & topology = *p_data.data.topology;
		for ( Index atom = 0; atom < topology.getAtomCount(); ++atom )
		{
			const Index residue = topology.getAtomResidueIndex( atom );
			const Core::ChemDB::Category::TYPE category = topology.residueCategories[ residue ];
			if ( not _isSesCategoryAllowed( category ) )
			{
				continue;
			}

			atomsByCategory[ size_t( category ) ].emplace_back( atom );
		}

		for ( size_t category = 0; category < atomsByCategory.size(); ++category )
		{
			if ( atomsByCategory[ category ].empty() )
			{
				continue;
			}

			const Surface surface = _getOrCreateSurface(
				SurfaceKey { p_handle, E_SES_COMPUTE_MODE::CATEGORY, uint32_t( category ) }
			);
			VTX_DEBUG(
				"SES category construction: {} surface={} atoms={} system={}",
				Core::ChemDB::Category::TYPE_STR[ category ],
				surface.id,
				atomsByCategory[ category ].size(),
				p_handle
			);
			_constructSurface(
				p_context,
				p_data,
				p_inputAtomOffset,
				p_probeRadius,
				p_computeMode,
				p_representation,
				surface,
				atomsByCategory[ category ]
			);
		}
	}

	void SES::_constructSurface(
		Context::ContextWrapper & p_context,
		const Cache::System &	  p_data,
		const uint32_t			  p_inputAtomOffset,
		const float				  p_probeRadius,
		const E_SES_COMPUTE_MODE  p_computeMode,
		const RepresentationIndex p_representation,
		const Surface &			  p_surface,
		std::span<const Index>	  p_atomIndices
	)
	{
		Util::ScopedChrono chrono( "SES construct" );

		auto construction				 = std::make_unique<SurfaceConstruction>();
		construction->surface			 = p_surface;
		construction->rendererAtomOffset = p_inputAtomOffset;
		construction->atomOffset		 = 0;
		construction->probeRadius		 = p_probeRadius;
		construction->computeMode		 = p_computeMode;

		for ( const auto & [ surfaceID, existingConstruction ] : _constructions )
		{
			construction->probeOffset = std::max(
				construction->probeOffset, existingConstruction->probeOffset + existingConstruction->probeNb
			);
			construction->sectorOffset = std::max(
				construction->sectorOffset, existingConstruction->sectorOffset + existingConstruction->sectorNb
			);
		}

		// TEMP BYPASS.
		constexpr bool bypassSES = false;
		if constexpr ( bypassSES )
		{
			_constructEmptyRanges( construction->surface );
			const SurfaceID surfaceID = construction->surface.id;
			_constructions.emplace( surfaceID, std::move( construction ) );
			return;
		}

		const Index topologyAtomCount = p_data.data.topology->getAtomCount();
		const Index atomCount		  = p_atomIndices.empty() ? topologyAtomCount : Index( p_atomIndices.size() );
		if ( topologyAtomCount != 0 && p_data.data.trajectory.size() != topologyAtomCount )
		{
			construction->markIncalculable();
			VTX_WARNING(
				"Can not build SES: atom count ({}) and position count ({}) mismatch.",
				topologyAtomCount,
				p_data.data.trajectory.size()
			);
		}
		else if ( atomCount != 0 )
		{
#ifdef VTX_CUDA_ENABLED
			if ( not p_context.isInteropAvailable( Desc::E_INTEROP_API::CUDA ) )
			{
				construction->markIncalculable();
				VTX_WARNING( "Can not build SES: CUDA graphics interop is not available." );
			}
			else
			{
				std::vector<Desc::InteropBufferMapping> mappings;

				try
				{
					SESDetail::CudaBuildResult result;
					if ( p_atomIndices.empty() )
					{
						const std::array<Desc::BufferRef, 4> buffers {
							Desc::BufferRef { Layout::Atoms::ATOMS_POSITIONS },
							Desc::BufferRef { Layout::Atoms::ATOMS_SYMBOLS },
							Desc::BufferRef { BUFFER_ATOMS, construction->surface.id },
							Desc::BufferRef { BUFFER_ATOM_IDS, construction->surface.id },
						};

						const Desc::BufferRef atomsRef { BUFFER_ATOMS, construction->surface.id };
						p_context.ensureBufferChunk( atomsRef );
						p_context.setBuffer<Vec4f>( atomsRef, std::max<uint32_t>( 1u, uint32_t( atomCount ) ) );

						const Desc::BufferRef atomIdsRef { BUFFER_ATOM_IDS, construction->surface.id };
						p_context.ensureBufferChunk( atomIdsRef );
						p_context.setBuffer<uint32_t>( atomIdsRef, std::max<uint32_t>( 1u, uint32_t( atomCount ) ) );

						mappings = p_context.mapInteropBuffers( Desc::E_INTEROP_API::CUDA, buffers );
						assert( mappings.size() == buffers.size() );

						SESDetail::SesdfInputBuffers inputs;
						inputs.positions		  = { mappings[ 0 ].devicePtr, mappings[ 0 ].size, 0 };
						inputs.symbols			  = { mappings[ 1 ].devicePtr, mappings[ 1 ].size, 0 };
						inputs.outputAtoms		  = { mappings[ 2 ].devicePtr, mappings[ 2 ].size, 0 };
						inputs.outputAtomIds	  = { mappings[ 3 ].devicePtr, mappings[ 3 ].size, 0 };
						inputs.atomOffset		  = p_inputAtomOffset;
						inputs.rendererAtomOffset = construction->rendererAtomOffset;
						inputs.atomNb			  = uint32_t( atomCount );

						result = SESDetail::buildCudaConstructionFromRendererBuffers(
							inputs, p_data.data.trajectory, p_probeRadius
						);
					}
					else
					{
						const std::array<Desc::BufferRef, 4> buffers {
							Desc::BufferRef { Layout::Atoms::ATOMS_POSITIONS },
							Desc::BufferRef { Layout::Atoms::ATOMS_SYMBOLS },
							Desc::BufferRef { BUFFER_ATOMS, construction->surface.id },
							Desc::BufferRef { BUFFER_ATOM_IDS, construction->surface.id },
						};

						const Desc::BufferRef atomsRef { BUFFER_ATOMS, construction->surface.id };
						p_context.ensureBufferChunk( atomsRef );
						p_context.setBuffer<Vec4f>( atomsRef, std::max<uint32_t>( 1u, uint32_t( atomCount ) ) );

						const Desc::BufferRef atomIdsRef { BUFFER_ATOM_IDS, construction->surface.id };
						p_context.ensureBufferChunk( atomIdsRef );
						p_context.setBuffer<uint32_t>( atomIdsRef, std::max<uint32_t>( 1u, uint32_t( atomCount ) ) );

						mappings = p_context.mapInteropBuffers( Desc::E_INTEROP_API::CUDA, buffers );
						assert( mappings.size() == buffers.size() );

						SESDetail::SesdfInputBuffers inputs;
						inputs.positions		  = { mappings[ 0 ].devicePtr, mappings[ 0 ].size, 0 };
						inputs.symbols			  = { mappings[ 1 ].devicePtr, mappings[ 1 ].size, 0 };
						inputs.outputAtoms		  = { mappings[ 2 ].devicePtr, mappings[ 2 ].size, 0 };
						inputs.outputAtomIds	  = { mappings[ 3 ].devicePtr, mappings[ 3 ].size, 0 };
						inputs.atomOffset		  = p_inputAtomOffset;
						inputs.rendererAtomOffset = construction->rendererAtomOffset;
						inputs.atomNb			  = uint32_t( atomCount );

						result = SESDetail::buildCudaConstructionFromRendererBuffers(
							inputs, p_data.data.trajectory, p_atomIndices, p_probeRadius
						);
					}

					construction->visibility.atomIds = SESDetail::readAtomIds(
						SESDetail::CudaBufferView { mappings[ 3 ].devicePtr, mappings[ 3 ].size, 0 }, result.atomNb
					);
					if ( result.construction != nullptr )
					{
						SESDetail::SesdfVisibilityData visibilityData;
						SESDetail::readConstructionVisibilityData( *result.construction, visibilityData );
						construction->visibility.circleAtoms	  = std::move( visibilityData.circleAtoms );
						construction->visibility.segmentAtoms = std::move( visibilityData.segmentAtoms );
						construction->visibility.concaveAtoms = std::move( visibilityData.concaveAtoms );
					}
					construction->cudaConstruction = std::move( result.construction );
					construction->atomNb		   = result.atomNb;
					construction->convexPatchNb	   = result.convexPatchNb;
					construction->circlePatchNb	   = result.circlePatchNb;
					construction->segmentPatchNb   = result.segmentPatchNb;
					construction->probeNb		   = result.probeNb;
					construction->sectorNb		   = result.sectorNb;
					construction->concavePatchNb   = result.concavePatchNb;
					construction->state			   = construction->cudaConstruction != nullptr
														 ? SurfaceConstruction::State::PendingWrite
														 : SurfaceConstruction::State::Incalculable;
				}
				catch ( const std::exception & p_e )
				{
					construction->markIncalculable();
					VTX_WARNING( "Can not build SES for system: {}", p_e.what() );
				}
				catch ( ... )
				{
					construction->markIncalculable();
					VTX_WARNING( "Can not build SES for system: unknown error" );
				}

				if ( not mappings.empty() )
				{
					p_context.unmapInteropBuffers( Desc::E_INTEROP_API::CUDA, mappings );
				}
				_unregisterCudaInputSourceBuffers( p_context );
				_unregisterCudaConstructionBuffers( p_context, construction->surface.id );
			}
#else
			construction->markIncalculable();
#endif
		}

		if ( construction->state == SurfaceConstruction::State::Incalculable )
		{
			_constructEmptyRanges( construction->surface );
		}
		else
		{
			const Surface & surface = construction->surface;
			convexPatches.construct(
				surface.id, surface.system, construction->convexPatchNb, p_representation, construction->atomOffset
			);
			circlePatches.construct( surface.id, surface.system, construction->circlePatchNb, p_representation );
			segmentPatches.construct( surface.id, surface.system, construction->segmentPatchNb, p_representation );
			concavePatches.construct( surface.id, surface.system, construction->concavePatchNb, p_representation, 0 );
		}

		const SurfaceID surfaceID = construction->surface.id;
		_constructions.emplace( surfaceID, std::move( construction ) );
	}

	bool SES::hasPendingCompute() const
	{
#ifdef VTX_CUDA_ENABLED
		for ( const auto & [ surfaceID, construction ] : _constructions )
		{
			if ( construction->pendingWrite() )
			{
				return true;
			}
		}
#endif

		return false;
	}

	void SES::resize( Context::ContextWrapper & p_context )
	{
		for ( const auto & [ surfaceID, construction ] : _constructions )
		{
			if ( construction->state != SurfaceConstruction::State::PendingWrite
				 && construction->state != SurfaceConstruction::State::Written )
			{
				continue;
			}

			const uint32_t chunk = construction->surface.id;

			Desc::BufferRef probesRef { BUFFER_PROBES, chunk };
			p_context.ensureBufferChunk( probesRef );
			p_context.setBuffer<Vec4f>( probesRef, std::max<uint32_t>( 1u, construction->probeNb ) );

			Desc::BufferRef probeAtomIndicesRef { BUFFER_PROBE_ATOM_INDICES, chunk };
			p_context.ensureBufferChunk( probeAtomIndicesRef );
			p_context.setBuffer<std::array<int32_t, 4>>(
				probeAtomIndicesRef, std::max<uint32_t>( 1u, construction->probeNb )
			);

			Desc::BufferRef probeNeighborsRef { BUFFER_PROBE_NEIGHBORS, chunk };
			p_context.ensureBufferChunk( probeNeighborsRef );
			p_context.setBuffer<Vec4f>(
				probeNeighborsRef, std::max<uint32_t>( 1u, construction->probeNb * MAX_PROBE_NEIGHBOR_NB )
			);

			Desc::BufferRef sectorsRef { BUFFER_SECTORS, chunk };
			p_context.ensureBufferChunk( sectorsRef );
			p_context.setBuffer<Vec4f>( sectorsRef, std::max<uint32_t>( 1u, construction->sectorNb ) );

			Desc::BufferRef convexRef { BUFFER_CONVEX_PATCH_ELEMENTS, chunk };
			p_context.ensureBufferChunk( convexRef );
			p_context.setBuffer<std::array<uint32_t, 2>>(
				convexRef, std::max<uint32_t>( 1u, construction->convexPatchNb )
			);

			Desc::BufferRef circleRef { BUFFER_CIRCLE_PATCH_ATOMS, chunk };
			p_context.ensureBufferChunk( circleRef );
			p_context.setBuffer<std::array<uint32_t, 2>>(
				circleRef, std::max<uint32_t>( 1u, construction->circlePatchNb )
			);

			Desc::BufferRef segmentRef { BUFFER_SEGMENT_PATCH_IDS, chunk };
			p_context.ensureBufferChunk( segmentRef );
			p_context.setBuffer<std::array<uint32_t, 4>>(
				segmentRef, std::max<uint32_t>( 1u, construction->segmentPatchNb )
			);
		}

		convexPatches.resize( p_context );
		circlePatches.resize( p_context );
		segmentPatches.resize( p_context );
		concavePatches.resize( p_context );
	}

	void SES::clear()
	{
		convexPatches.clear();
		circlePatches.clear();
		segmentPatches.clear();
		concavePatches.clear();

		_constructions.clear();
		_surfaces.clear();
	}

	void SES::remove( Context::ContextWrapper & p_context, const Desc::Handle p_handle )
	{
		const auto it = _surfaces.bySystem.find( p_handle );
		if ( it == _surfaces.bySystem.end() )
		{
			return;
		}

		for ( const SurfaceID surface : it->second )
		{
			_releaseChunks( p_context, surface );
		}

		invalidate( p_handle );
	}

	void SES::invalidate( const Desc::Handle p_handle )
	{
		const auto it = _surfaces.bySystem.find( p_handle );
		if ( it == _surfaces.bySystem.end() )
		{
			return;
		}

		for ( const SurfaceID surface : it->second )
		{
			convexPatches.remove( surface );
			circlePatches.remove( surface );
			segmentPatches.remove( surface );
			concavePatches.remove( surface );
			_constructions.erase( surface );
		}

		for ( auto surfaceIt = _surfaces.ids.begin(); surfaceIt != _surfaces.ids.end(); )
		{
			if ( surfaceIt->first.system == p_handle )
			{
				surfaceIt = _surfaces.ids.erase( surfaceIt );
			}
			else
			{
				++surfaceIt;
			}
		}

		_surfaces.bySystem.erase( it );
	}

	void SES::invalidateForRecompute( const Desc::Handle p_handle )
	{
		const auto it = _surfaces.bySystem.find( p_handle );
		if ( it == _surfaces.bySystem.end() )
		{
			return;
		}

		for ( const SurfaceID surface : it->second )
		{
			convexPatches.remove( surface );
			circlePatches.remove( surface );
			segmentPatches.remove( surface );
			concavePatches.remove( surface );
			_constructions.erase( surface );
		}
	}

	void SES::uploadIndexes( Context::ContextWrapper & p_context, const Desc::Handle p_handle )
	{
		const auto it = _surfaces.bySystem.find( p_handle );
		if ( it == _surfaces.bySystem.end() )
		{
			return;
		}

		for ( const SurfaceID surface : it->second )
		{
			if ( not convexPatches.contains( surface ) || not circlePatches.contains( surface )
				 || not segmentPatches.contains( surface ) || not concavePatches.contains( surface ) )
			{
				continue;
			}

			convexPatches.uploadIndexes( p_context, surface );
			circlePatches.uploadIndexes( p_context, surface );
			segmentPatches.uploadIndexes( p_context, surface );
			concavePatches.uploadIndexes( p_context, surface );
		}
	}

	bool SES::built( const Desc::Handle p_handle ) const
	{
		const auto surfaceIt = _surfaces.bySystem.find( p_handle );
		if ( surfaceIt == _surfaces.bySystem.end() )
		{
			return false;
		}

		for ( const SurfaceID surface : surfaceIt->second )
		{
			if ( _constructions.contains( surface ) )
			{
				return true;
			}
		}

		return false;
	}

	float SES::probeRadius( const Desc::Handle p_handle ) const
	{
		const auto surfaceIt = _surfaces.bySystem.find( p_handle );
		assert( surfaceIt != _surfaces.bySystem.end() );
		assert( not surfaceIt->second.empty() );

		for ( const SurfaceID surface : surfaceIt->second )
		{
			const auto constructionIt = _constructions.find( surface );
			if ( constructionIt != _constructions.end() )
			{
				return constructionIt->second->probeRadius;
			}
		}

		return SES_PROBE_RADIUS_DEFAULT;
	}

	E_SES_COMPUTE_MODE SES::computeMode( const Desc::Handle p_handle ) const
	{
		const auto surfaceIt = _surfaces.bySystem.find( p_handle );
		if ( surfaceIt == _surfaces.bySystem.end() || surfaceIt->second.empty() )
		{
			return SES_COMPUTE_MODE_DEFAULT;
		}

		for ( const SurfaceID surface : surfaceIt->second )
		{
			const auto constructionIt = _constructions.find( surface );
			if ( constructionIt != _constructions.end() )
			{
				return constructionIt->second->computeMode;
			}
		}

		return SES_COMPUTE_MODE_DEFAULT;
	}

	void SES::setVisibility( const Desc::Handle p_handle, const bool p_visible )
	{
		const auto it = _surfaces.bySystem.find( p_handle );
		if ( it == _surfaces.bySystem.end() )
		{
			return;
		}

		for ( const SurfaceID surface : it->second )
		{
			if ( not convexPatches.contains( surface ) || not circlePatches.contains( surface )
				 || not segmentPatches.contains( surface ) || not concavePatches.contains( surface ) )
			{
				continue;
			}

			convexPatches.setVisibility( surface, p_visible );
			circlePatches.setVisibility( surface, p_visible );
			segmentPatches.setVisibility( surface, p_visible );
			concavePatches.setVisibility( surface, p_visible );
		}
	}

	void SES::setVisibility( const Desc::Handle p_handle, const Util::Math::BitSet & p_visibility )
	{
		const auto it = _surfaces.bySystem.find( p_handle );
		if ( it == _surfaces.bySystem.end() )
		{
			return;
		}

		for ( const SurfaceID surface : it->second )
		{
			if ( not convexPatches.contains( surface ) || not circlePatches.contains( surface )
				 || not segmentPatches.contains( surface ) || not concavePatches.contains( surface ) )
			{
				continue;
			}

			const auto constructionIt = _constructions.find( surface );
			if ( constructionIt == _constructions.end() )
			{
				continue;
			}

			const SurfaceConstruction & construction = *constructionIt->second;
			if ( p_visibility.none() )
			{
				convexPatches.setVisibility( surface, false );
				circlePatches.setVisibility( surface, false );
				segmentPatches.setVisibility( surface, false );
				concavePatches.setVisibility( surface, false );
				continue;
			}

			if ( construction.visibility.atomIds.size() >= construction.convexPatchNb )
			{
				convexPatches.setIndices(
					surface,
					_filterPatchIndices(
						construction.convexPatchNb,
						[ & ]( const uint32_t p_patch )
						{
							return _isSortedAtomVisible( construction, p_patch, p_visibility );
						}
					)
				);
			}
			else
			{
				convexPatches.setVisibility( surface, true );
			}

			if ( construction.visibility.circleAtoms.size() == construction.circlePatchNb )
			{
				circlePatches.setIndices(
					surface,
					_filterPatchIndices(
						construction.circlePatchNb,
						[ & ]( const uint32_t p_patch )
						{
							const auto & atoms = construction.visibility.circleAtoms[ p_patch ];
							return _isSortedAtomVisible( construction, atoms[ 0 ], p_visibility )
								   && _isSortedAtomVisible( construction, atoms[ 1 ], p_visibility );
						}
					)
				);
			}
			else
			{
				circlePatches.setVisibility( surface, true );
			}

			if ( construction.visibility.segmentAtoms.size() == construction.segmentPatchNb )
			{
				segmentPatches.setIndices(
					surface,
					_filterPatchIndices(
						construction.segmentPatchNb,
						[ & ]( const uint32_t p_patch )
						{
							const auto & atoms = construction.visibility.segmentAtoms[ p_patch ];
							return _isSortedAtomVisible( construction, atoms[ 0 ], p_visibility )
								   && _isSortedAtomVisible( construction, atoms[ 1 ], p_visibility );
						}
					)
				);
			}
			else
			{
				segmentPatches.setVisibility( surface, true );
			}

			if ( construction.visibility.concaveAtoms.size() == construction.concavePatchNb )
			{
				concavePatches.setIndices(
					surface,
					_filterPatchIndices(
						construction.concavePatchNb,
						[ & ]( const uint32_t p_patch )
						{
							const auto & atoms = construction.visibility.concaveAtoms[ p_patch ];
							return atoms[ 0 ] >= 0 && atoms[ 1 ] >= 0 && atoms[ 2 ] >= 0
								   && _isSortedAtomVisible( construction, uint32_t( atoms[ 0 ] ), p_visibility )
								   && _isSortedAtomVisible( construction, uint32_t( atoms[ 1 ] ), p_visibility )
								   && _isSortedAtomVisible( construction, uint32_t( atoms[ 2 ] ), p_visibility );
						}
					)
				);
			}
			else
			{
				concavePatches.setVisibility( surface, true );
			}
		}
	}

	void SES::compute( Context::ContextWrapper & p_context )
	{
		Util::ScopedChrono chrono( "SES compute" );

#ifdef VTX_CUDA_ENABLED
		if ( not hasPendingCompute() )
		{
			return;
		}

		if ( not p_context.isInteropAvailable( Desc::E_INTEROP_API::CUDA ) )
		{
			VTX_WARNING( "Can not compute SES: CUDA graphics interop is not available." );
			return;
		}

		for ( const auto & [ surfaceID, construction ] : _constructions )
		{
			if ( not construction->pendingWrite() )
			{
				continue;
			}

			std::vector<Desc::InteropBufferMapping> mappings;

			try
			{
				const std::vector<Desc::BufferRef> buffers = _outputBufferRefs( *construction );
				mappings = p_context.mapInteropBuffers( Desc::E_INTEROP_API::CUDA, buffers );
				assert( mappings.size() == buffers.size() );

				SESDetail::writeCudaConstruction(
					*construction->cudaConstruction, _outputBuffers( mappings, *construction )
				);
				p_context.unmapInteropBuffers( Desc::E_INTEROP_API::CUDA, mappings );
				mappings.clear();

				construction->cudaConstruction.reset();
				construction->state = SurfaceConstruction::State::Written;
				_unregisterCudaSurfaceBuffers( p_context, surfaceID );
			}
			catch ( const std::exception & p_e )
			{
				if ( not mappings.empty() )
				{
					p_context.unmapInteropBuffers( Desc::E_INTEROP_API::CUDA, mappings );
				}

				_unregisterCudaSurfaceBuffers( p_context, surfaceID );
				_disableDraws( p_context, surfaceID );
				construction->markIncalculable();
				VTX_WARNING( "Can not write SES renderer buffers: {}", p_e.what() );
			}
			catch ( ... )
			{
				if ( not mappings.empty() )
				{
					p_context.unmapInteropBuffers( Desc::E_INTEROP_API::CUDA, mappings );
				}

				_unregisterCudaSurfaceBuffers( p_context, surfaceID );
				_disableDraws( p_context, surfaceID );
				construction->markIncalculable();
				VTX_WARNING( "Can not write SES renderer buffers: unknown error." );
			}
		}

#endif
	}

	SES::Surface SES::_createSurface( const SurfaceKey & p_key )
	{
		const auto existing = _surfaces.ids.find( p_key );
		assert( existing == _surfaces.ids.end() );

		Surface surface;
		surface.id	   = _surfaces.nextID++;
		surface.system = p_key.system;
		surface.scope  = p_key.scope;
		surface.index  = p_key.index;

		_surfaces.ids.emplace( p_key, surface.id );
		_surfaces.bySystem[ p_key.system ].emplace_back( surface.id );
		return surface;
	}

	SES::Surface SES::_createWholeSurface( const Desc::Handle p_handle )
	{
		return _createSurface( SurfaceKey { p_handle, E_SES_COMPUTE_MODE::SYSTEM, 0 } );
	}

	SES::Surface SES::_getOrCreateSurface( const SurfaceKey & p_key )
	{
		const auto it = _surfaces.ids.find( p_key );
		if ( it == _surfaces.ids.end() )
		{
			return _createSurface( p_key );
		}

		return Surface { it->second, p_key.system, p_key.scope, p_key.index };
	}

	SES::Surface SES::_getOrCreateWholeSurface( const Desc::Handle p_handle )
	{
		return _getOrCreateSurface( SurfaceKey { p_handle, E_SES_COMPUTE_MODE::SYSTEM, 0 } );
	}

	void SES::_releaseChunks( Context::ContextWrapper & p_context, const SurfaceID p_surface )
	{
		const std::array<Desc::Key, 16> buffers {
			BUFFER_ATOMS,
			BUFFER_ATOM_IDS,
			BUFFER_SECTORS,
			BUFFER_PROBES,
			BUFFER_PROBE_ATOM_INDICES,
			BUFFER_PROBE_NEIGHBORS,
			BUFFER_CONVEX_PATCH_ELEMENTS,
			BUFFER_CIRCLE_PATCH_ATOMS,
			BUFFER_SEGMENT_PATCH_IDS,
			INDEX_CONVEX_PATCHES,
			INDEX_CIRCLE_PATCHES,
			INDEX_SEGMENT_PATCHES,
			INDEX_CONCAVE_PATCHES,
			INDIRECT_CONVEX_PATCHES,
			INDIRECT_CIRCLE_PATCHES,
			INDIRECT_SEGMENT_PATCHES,
		};

		for ( const Desc::Key & buffer : buffers )
		{
			p_context.releaseBufferChunk( { buffer, p_surface } );
		}
		p_context.releaseBufferChunk( { INDIRECT_CONCAVE_PATCHES, p_surface } );
	}

	void SES::_unregisterCudaInputSourceBuffers( Context::ContextWrapper & p_context )
	{
#ifdef VTX_CUDA_ENABLED
		p_context.unregisterInteropBuffer( Desc::E_INTEROP_API::CUDA, { Layout::Atoms::ATOMS_POSITIONS } );
		p_context.unregisterInteropBuffer( Desc::E_INTEROP_API::CUDA, { Layout::Atoms::ATOMS_SYMBOLS } );
#endif
	}

	void SES::_unregisterCudaConstructionBuffers( Context::ContextWrapper & p_context, const SurfaceID p_surface )
	{
#ifdef VTX_CUDA_ENABLED
		p_context.unregisterInteropBuffer( Desc::E_INTEROP_API::CUDA, { BUFFER_ATOMS, p_surface } );
		p_context.unregisterInteropBuffer( Desc::E_INTEROP_API::CUDA, { BUFFER_ATOM_IDS, p_surface } );
#endif
	}

	void SES::_unregisterCudaSurfaceBuffers( Context::ContextWrapper & p_context, const SurfaceID p_surface )
	{
#ifdef VTX_CUDA_ENABLED
		const std::array<Desc::BufferRef, OUTPUT_BUFFER_NB + 2> buffers = {
			Desc::BufferRef { BUFFER_ATOMS, p_surface },
			Desc::BufferRef { BUFFER_ATOM_IDS, p_surface },
			Desc::BufferRef { BUFFER_CONVEX_PATCH_ELEMENTS, p_surface },
			Desc::BufferRef { BUFFER_CIRCLE_PATCH_ATOMS, p_surface },
			Desc::BufferRef { BUFFER_SEGMENT_PATCH_IDS, p_surface },
			Desc::BufferRef { BUFFER_PROBES, p_surface },
			Desc::BufferRef { BUFFER_PROBE_ATOM_INDICES, p_surface },
			Desc::BufferRef { BUFFER_PROBE_NEIGHBORS, p_surface },
			Desc::BufferRef { BUFFER_SECTORS, p_surface },
		};

		for ( const Desc::BufferRef & buffer : buffers )
		{
			p_context.unregisterInteropBuffer( Desc::E_INTEROP_API::CUDA, buffer );
		}
#endif
	}

	void SES::_constructEmptyRanges( const Surface & p_surface )
	{
		convexPatches.construct( p_surface.id, p_surface.system, 0, 0 );
		circlePatches.construct( p_surface.id, p_surface.system, 0, 0 );
		segmentPatches.construct( p_surface.id, p_surface.system, 0, 0 );
		concavePatches.construct( p_surface.id, p_surface.system, 0, 0 );
	}

	void SES::_disableDraws( Context::ContextWrapper & p_context, const SurfaceID p_surface )
	{
		BinaryBuffer430 buffer = _emptyIndirectBuffer();

		auto disable = [ & ]( const Desc::Key & p_indirectBuffer, const PatchGeometry & p_geometry )
		{
			if ( std::find( p_geometry.chunks.begin(), p_geometry.chunks.end(), p_surface ) == p_geometry.chunks.end() )
			{
				return;
			}

			const Desc::BufferRef ref { p_indirectBuffer, p_surface };
			p_context.ensureBufferChunk( ref );
			p_context.setBuffer( ref, buffer );
		};

		disable( INDIRECT_CONVEX_PATCHES, convexPatches );
		disable( INDIRECT_CIRCLE_PATCHES, circlePatches );
		disable( INDIRECT_SEGMENT_PATCHES, segmentPatches );
		disable( INDIRECT_CONCAVE_PATCHES, concavePatches );
	}

} // namespace VTX::Renderer::Geometry
