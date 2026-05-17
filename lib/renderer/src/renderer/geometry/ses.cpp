#include "renderer/geometry/ses.hpp"
#include "renderer/representation.hpp"
#include <array>
#include <util/chrono.hpp>
#include <util/logger.hpp>
#ifdef VTX_CUDA_ENABLED
#include "renderer/geometry/ses_cuda.hpp"
#endif

namespace VTX::Renderer::Geometry
{
	struct SES::Construction
	{
#ifdef VTX_CUDA_ENABLED
		SESDetail::CudaConstructionPtr cudaConstruction;
#endif

		uint32_t atomOffset		= 0;
		uint32_t atomNb			= 0;
		uint32_t probeOffset	= 0;
		uint32_t sectorOffset	= 0;
		uint32_t convexPatchNb	= 0;
		uint32_t circlePatchNb	= 0;
		uint32_t segmentPatchNb = 0;
		uint32_t probeNb		= 0;
		uint32_t sectorNb		= 0;
		uint32_t concavePatchNb = 0;
	};

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

	void SES::construct( const Desc::Handle p_handle, const SystemData & p_data )
	{
		Util::ScopedChrono chrono( "SES construct" );

		auto construction = std::make_unique<Construction>();
		for ( const auto & [ handle, existingConstruction ] : _construction )
		{
			construction->atomOffset += existingConstruction->atomNb;
			construction->probeOffset += existingConstruction->probeNb;
			construction->sectorOffset += existingConstruction->sectorNb;
		}

		// Temporary SES bypass: keep empty geometry ranges so the render graph stays valid, but skip the CUDA build.
		convexPatches.construct( p_handle, 0 );
		circlePatches.construct( p_handle, 0 );
		segmentPatches.construct( p_handle, 0 );
		concavePatches.construct( p_handle, 0 );

		_construction.emplace( p_handle, std::move( construction ) );
		return;

		const Index atomCount = p_data.data.getAtomCount();
		if ( atomCount != 0 && p_data.trajectory.size() != atomCount )
		{
			VTX_WARNING(
				"Can not build SES for system {}: atom count ({}) and position count ({}) mismatch.",
				p_data.uid,
				atomCount,
				p_data.trajectory.size()
			);
		}
		else if ( atomCount != 0 )
		{
#ifdef VTX_CUDA_ENABLED
			SESDetail::CudaBuildResult result = SESDetail::buildCudaConstruction(
				p_data.trajectory, p_data.data.atomSymbols, SES_PROBE_RADIUS_DEFAULT
			);

			construction->cudaConstruction = std::move( result.construction );
			construction->atomNb		   = result.atomNb;
			construction->convexPatchNb	   = result.convexPatchNb;
			construction->circlePatchNb	   = result.circlePatchNb;
			construction->segmentPatchNb   = result.segmentPatchNb;
			construction->probeNb		   = result.probeNb;
			construction->sectorNb		   = result.sectorNb;
			construction->concavePatchNb   = result.concavePatchNb;
#endif
		}

		convexPatches.construct( p_handle, construction->convexPatchNb );
		circlePatches.construct( p_handle, construction->circlePatchNb );
		segmentPatches.construct( p_handle, construction->segmentPatchNb );
		concavePatches.construct( p_handle, construction->concavePatchNb );

		_construction.emplace( p_handle, std::move( construction ) );
	}

	void SES::resize( Context::ContextWrapper & p_context )
	{
		uint32_t atomNb	  = 0;
		uint32_t probeNb  = 0;
		uint32_t sectorNb = 0;
		for ( const auto & [ handle, construction ] : _construction )
		{
			atomNb += construction->atomNb;
			probeNb += construction->probeNb;
			sectorNb += construction->sectorNb;
		}

		p_context.setBuffer<Vec4f>( BUFFER_ATOMS, std::max<uint32_t>( 1u, atomNb ) );
		p_context.setBuffer<Vec4f>( BUFFER_PROBES, std::max<uint32_t>( 1u, probeNb ) );
		p_context.setBuffer<std::array<int32_t, 4>>( BUFFER_PROBE_ATOM_INDICES, std::max<uint32_t>( 1u, probeNb ) );
		p_context.setBuffer<Vec4f>( BUFFER_PROBE_NEIGHBORS, std::max<uint32_t>( 1u, probeNb * MAX_PROBE_NEIGHBOR_NB ) );
		p_context.setBuffer<Vec4f>( BUFFER_SECTORS, std::max<uint32_t>( 1u, sectorNb ) );
		p_context.setBuffer<std::array<uint32_t, 2>>(
			BUFFER_CONVEX_PATCH_ELEMENTS, std::max<uint32_t>( 1u, convexPatches.totalSize )
		);
		p_context.setBuffer<std::array<uint32_t, 2>>(
			BUFFER_CIRCLE_PATCH_ATOMS, std::max<uint32_t>( 1u, circlePatches.totalSize )
		);
		p_context.setBuffer<std::array<uint32_t, 4>>(
			BUFFER_SEGMENT_PATCH_IDS, std::max<uint32_t>( 1u, segmentPatches.totalSize )
		);

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

		_construction.clear();
	}

	void SES::uploadIndexes( Context::ContextWrapper & p_context, const Desc::Handle p_handle )
	{
		convexPatches.uploadIndexes( p_context, p_handle );
		circlePatches.uploadIndexes( p_context, p_handle );
		segmentPatches.uploadIndexes( p_context, p_handle );
		concavePatches.uploadIndexes( p_context, p_handle );
	}

	void SES::compute( Context::ContextWrapper & p_context )
	{
		Util::ScopedChrono chrono( "SES compute" );
		return;

#ifdef VTX_CUDA_ENABLED
		bool hasCudaConstruction = false;
		for ( const auto & entry : _construction )
		{
			if ( entry.second->cudaConstruction != nullptr )
			{
				hasCudaConstruction = true;
				break;
			}
		}

		if ( not hasCudaConstruction )
		{
			return;
		}

		if ( not p_context.isInteropAvailable( Desc::E_INTEROP_API::CUDA ) )
		{
			VTX_WARNING( "Can not compute SES: CUDA graphics interop is not available." );
			return;
		}

		const std::array<Desc::Key, 8> keys {
			BUFFER_ATOMS,  BUFFER_CONVEX_PATCH_ELEMENTS, BUFFER_CIRCLE_PATCH_ATOMS, BUFFER_SEGMENT_PATCH_IDS,
			BUFFER_PROBES, BUFFER_PROBE_ATOM_INDICES,	 BUFFER_PROBE_NEIGHBORS,	BUFFER_SECTORS,
		};
		std::vector<Desc::InteropBufferMapping> mappings
			= p_context.mapInteropBuffers( Desc::E_INTEROP_API::CUDA, keys );
		assert( mappings.size() == keys.size() );

		for ( const auto & [ handle, construction ] : _construction )
		{
			if ( construction->cudaConstruction == nullptr )
			{
				continue;
			}

			SESDetail::SesdfRenderBuffers renderBuffers;
			renderBuffers.atoms
				= { mappings[ 0 ].devicePtr, mappings[ 0 ].size, construction->atomOffset * sizeof( Vec4f ) };
			renderBuffers.sectors
				= { mappings[ 7 ].devicePtr, mappings[ 7 ].size, construction->sectorOffset * sizeof( Vec4f ) };
			renderBuffers.convexPatches	 = { mappings[ 1 ].devicePtr,
											 mappings[ 1 ].size,
											 convexPatches.offset( handle ) * sizeof( std::array<uint32_t, 2> ) };
			renderBuffers.circlePatches	 = { mappings[ 2 ].devicePtr,
											 mappings[ 2 ].size,
											 circlePatches.offset( handle ) * sizeof( std::array<uint32_t, 2> ) };
			renderBuffers.segmentPatches = { mappings[ 3 ].devicePtr,
											 mappings[ 3 ].size,
											 segmentPatches.offset( handle ) * sizeof( std::array<uint32_t, 4> ) };
			renderBuffers.probes
				= { mappings[ 4 ].devicePtr, mappings[ 4 ].size, construction->probeOffset * sizeof( Vec4f ) };
			renderBuffers.probeAtomIndices	 = { mappings[ 5 ].devicePtr,
												 mappings[ 5 ].size,
												 construction->probeOffset * sizeof( std::array<int32_t, 4> ) };
			renderBuffers.probeNeighbors	 = { mappings[ 6 ].devicePtr,
												 mappings[ 6 ].size,
												 construction->probeOffset * MAX_PROBE_NEIGHBOR_NB * sizeof( Vec4f ) };
			renderBuffers.atomIndexOffset	 = construction->atomOffset;
			renderBuffers.probeIndexOffset	 = construction->probeOffset;
			renderBuffers.sectorIndexOffset	 = construction->sectorOffset;
			renderBuffers.maxProbeNeighborNb = MAX_PROBE_NEIGHBOR_NB;

			SESDetail::writeCudaConstruction( *construction->cudaConstruction, renderBuffers );
		}

		p_context.unmapInteropBuffers( Desc::E_INTEROP_API::CUDA, mappings );
#endif
	}
} // namespace VTX::Renderer::Geometry
