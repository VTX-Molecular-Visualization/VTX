#include "renderer/geometry/ses.hpp"
#include "renderer/binary_buffer.hpp"
#include "renderer/layout/atoms.hpp"
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

		State	 state			= State::Empty;
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

#ifdef VTX_CUDA_ENABLED
		SESDetail::SesdfRenderBuffers _renderBuffers(
			std::span<const Desc::InteropBufferMapping> p_mappings,
			const SES::Construction &					 p_construction,
			const Desc::Handle							 p_handle,
			const SES::PatchGeometry &					 p_convexPatches,
			const SES::PatchGeometry &					 p_circlePatches,
			const SES::PatchGeometry &					 p_segmentPatches
		)
		{
			SESDetail::SesdfRenderBuffers renderBuffers;
			renderBuffers.atoms
				= { p_mappings[ 0 ].devicePtr, p_mappings[ 0 ].size, p_construction.atomOffset * sizeof( Vec4f ) };
			renderBuffers.convexPatches
				= { p_mappings[ 1 ].devicePtr,
					p_mappings[ 1 ].size,
					p_convexPatches.offset( p_handle ) * sizeof( std::array<uint32_t, 2> ) };
			renderBuffers.circlePatches
				= { p_mappings[ 2 ].devicePtr,
					p_mappings[ 2 ].size,
					p_circlePatches.offset( p_handle ) * sizeof( std::array<uint32_t, 2> ) };
			renderBuffers.segmentPatches
				= { p_mappings[ 3 ].devicePtr,
					p_mappings[ 3 ].size,
					p_segmentPatches.offset( p_handle ) * sizeof( std::array<uint32_t, 4> ) };
			renderBuffers.probes
				= { p_mappings[ 4 ].devicePtr, p_mappings[ 4 ].size, p_construction.probeOffset * sizeof( Vec4f ) };
			renderBuffers.probeAtomIndices
				= { p_mappings[ 5 ].devicePtr,
					p_mappings[ 5 ].size,
					p_construction.probeOffset * sizeof( std::array<int32_t, 4> ) };
			renderBuffers.probeNeighbors
				= { p_mappings[ 6 ].devicePtr,
					p_mappings[ 6 ].size,
					p_construction.probeOffset * SES::MAX_PROBE_NEIGHBOR_NB * sizeof( Vec4f ) };
			renderBuffers.sectors
				= { p_mappings[ 7 ].devicePtr, p_mappings[ 7 ].size, p_construction.sectorOffset * sizeof( Vec4f ) };
			renderBuffers.atomIndexOffset	 = p_construction.atomOffset;
			renderBuffers.probeIndexOffset	 = p_construction.probeOffset;
			renderBuffers.sectorIndexOffset	 = p_construction.sectorOffset;
			renderBuffers.maxProbeNeighborNb = SES::MAX_PROBE_NEIGHBOR_NB;

			return renderBuffers;
		}
#endif
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
		const SystemData &		  p_data,
		const uint32_t			  p_inputAtomOffset
	)
	{
		Util::ScopedChrono chrono( "SES construct" );

		auto construction = std::make_unique<Construction>();
		for ( const auto & [ handle, existingConstruction ] : _construction )
		{
			construction->atomOffset += existingConstruction->atomNb;
			construction->probeOffset += existingConstruction->probeNb;
			construction->sectorOffset += existingConstruction->sectorNb;
		}

		// TEMP BYPASS.
		constexpr bool bypassSES = false;
		if constexpr ( bypassSES )
		{
			_constructEmptyRanges( p_handle );
			_construction.emplace( p_handle, std::move( construction ) );
			return;
		}

		const Index atomCount = p_data.data.getAtomCount();
		if ( atomCount != 0 && p_data.trajectory.size() != atomCount )
		{
			construction->markIncalculable();
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
			if ( not p_context.isInteropAvailable( Desc::E_INTEROP_API::CUDA ) )
			{
				construction->markIncalculable();
				VTX_WARNING( "Can not build SES: CUDA graphics interop is not available." );
			}
			else
			{
				const std::array<Desc::Key, 2> keys {
					Layout::Atoms::ATOMS_POSITIONS,
					Layout::Atoms::ATOMS_SYMBOLS,
				};

				std::vector<Desc::InteropBufferMapping> mappings;

				try
				{
					mappings = p_context.mapInteropBuffers( Desc::E_INTEROP_API::CUDA, keys );
					assert( mappings.size() == keys.size() );

					SESDetail::SesdfInputBuffers inputs;
					inputs.positions  = { mappings[ 0 ].devicePtr, mappings[ 0 ].size, 0 };
					inputs.symbols	  = { mappings[ 1 ].devicePtr, mappings[ 1 ].size, 0 };
					inputs.atomOffset = p_inputAtomOffset;
					inputs.atomNb	  = uint32_t( atomCount );

					SESDetail::CudaBuildResult result = SESDetail::buildCudaConstructionFromRendererBuffers(
						inputs, p_data.trajectory, SES_PROBE_RADIUS_DEFAULT
					);

					construction->cudaConstruction = std::move( result.construction );
					construction->atomNb		   = result.atomNb;
					construction->convexPatchNb	   = result.convexPatchNb;
					construction->circlePatchNb	   = result.circlePatchNb;
					construction->segmentPatchNb   = result.segmentPatchNb;
					construction->probeNb		   = result.probeNb;
					construction->sectorNb		   = result.sectorNb;
					construction->concavePatchNb   = result.concavePatchNb;
					construction->state = construction->cudaConstruction != nullptr
											  ? Construction::State::PendingWrite
											  : Construction::State::Incalculable;
				}
				catch ( const std::exception & p_e )
				{
					construction->markIncalculable();
					VTX_WARNING( "Can not build SES for system {}: {}", p_data.uid, p_e.what() );
				}
				catch ( ... )
				{
					construction->markIncalculable();
					VTX_WARNING( "Can not build SES for system {}: unknown error.", p_data.uid );
				}

				if ( not mappings.empty() )
				{
					p_context.unmapInteropBuffers( Desc::E_INTEROP_API::CUDA, mappings );
				}
			}
#else
			construction->markIncalculable();
#endif
		}

		if ( construction->state == Construction::State::Incalculable )
		{
			_constructEmptyRanges( p_handle );
		}
		else
		{
			convexPatches.construct( p_handle, construction->convexPatchNb );
			circlePatches.construct( p_handle, construction->circlePatchNb );
			segmentPatches.construct( p_handle, construction->segmentPatchNb );
			concavePatches.construct( p_handle, construction->concavePatchNb );
		}

		_construction.emplace( p_handle, std::move( construction ) );
	}

	bool SES::hasPendingCompute() const
	{
#ifdef VTX_CUDA_ENABLED
		for ( const auto & [ handle, construction ] : _construction )
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

		const std::array<Desc::Key, 8> keys {
			BUFFER_ATOMS,  BUFFER_CONVEX_PATCH_ELEMENTS, BUFFER_CIRCLE_PATCH_ATOMS, BUFFER_SEGMENT_PATCH_IDS,
			BUFFER_PROBES, BUFFER_PROBE_ATOM_INDICES,	 BUFFER_PROBE_NEIGHBORS,	BUFFER_SECTORS,
		};

		std::vector<Desc::InteropBufferMapping> mappings;

		try
		{
			mappings = p_context.mapInteropBuffers( Desc::E_INTEROP_API::CUDA, keys );
			assert( mappings.size() == keys.size() );

			for ( const auto & [ handle, construction ] : _construction )
			{
				if ( not construction->pendingWrite() )
				{
					continue;
				}

				SESDetail::writeCudaConstruction(
					*construction->cudaConstruction,
					_renderBuffers( mappings, *construction, handle, convexPatches, circlePatches, segmentPatches )
				);
				construction->cudaConstruction.reset();
				construction->state = Construction::State::Written;
			}
		}
		catch ( const std::exception & p_e )
		{
			if ( not mappings.empty() )
			{
				p_context.unmapInteropBuffers( Desc::E_INTEROP_API::CUDA, mappings );
			}
			_discardPendingCompute( p_context );
			VTX_WARNING( "Can not write SES renderer buffers: {}", p_e.what() );
			return;
		}
		catch ( ... )
		{
			if ( not mappings.empty() )
			{
				p_context.unmapInteropBuffers( Desc::E_INTEROP_API::CUDA, mappings );
			}
			_discardPendingCompute( p_context );
			VTX_WARNING( "Can not write SES renderer buffers: unknown error." );
			return;
		}

		p_context.unmapInteropBuffers( Desc::E_INTEROP_API::CUDA, mappings );
#endif
	}

	void SES::_constructEmptyRanges( const Desc::Handle p_handle )
	{
		convexPatches.construct( p_handle, 0 );
		circlePatches.construct( p_handle, 0 );
		segmentPatches.construct( p_handle, 0 );
		concavePatches.construct( p_handle, 0 );
	}

	void SES::_clearPatchGeometries()
	{
		convexPatches.clear();
		circlePatches.clear();
		segmentPatches.clear();
		concavePatches.clear();
	}

	void SES::_releaseBuffers( Context::ContextWrapper & p_context )
	{
		p_context.setBuffer<Vec4f>( BUFFER_ATOMS, 1 );
		p_context.setBuffer<Vec4f>( BUFFER_PROBES, 1 );
		p_context.setBuffer<std::array<int32_t, 4>>( BUFFER_PROBE_ATOM_INDICES, 1 );
		p_context.setBuffer<Vec4f>( BUFFER_PROBE_NEIGHBORS, 1 );
		p_context.setBuffer<Vec4f>( BUFFER_SECTORS, 1 );
		p_context.setBuffer<std::array<uint32_t, 2>>( BUFFER_CONVEX_PATCH_ELEMENTS, 1 );
		p_context.setBuffer<std::array<uint32_t, 2>>( BUFFER_CIRCLE_PATCH_ATOMS, 1 );
		p_context.setBuffer<std::array<uint32_t, 4>>( BUFFER_SEGMENT_PATCH_IDS, 1 );

		p_context.setBuffer<Indice>( INDEX_CONVEX_PATCHES, 1 );
		p_context.setBuffer<Indice>( INDEX_CIRCLE_PATCHES, 1 );
		p_context.setBuffer<Indice>( INDEX_SEGMENT_PATCHES, 1 );
		p_context.setBuffer<Indice>( INDEX_CONCAVE_PATCHES, 1 );
	}

	void SES::_disableDraws( Context::ContextWrapper & p_context )
	{
		BinaryBuffer430 buffer = _emptyIndirectBuffer();

		p_context.setBuffer( INDIRECT_CONVEX_PATCHES, buffer );
		p_context.setBuffer( INDIRECT_CIRCLE_PATCHES, buffer );
		p_context.setBuffer( INDIRECT_SEGMENT_PATCHES, buffer );
		p_context.setBuffer( INDIRECT_CONCAVE_PATCHES, buffer );
	}

	void SES::_markPendingConstructionsAsIncalculable()
	{
		for ( auto & [ handle, construction ] : _construction )
		{
			if ( construction->state == Construction::State::PendingWrite )
			{
				construction->markIncalculable();
			}
		}
	}

	void SES::_discardPendingCompute( Context::ContextWrapper & p_context )
	{
		_markPendingConstructionsAsIncalculable();
		_clearPatchGeometries();
		_releaseBuffers( p_context );
		_disableDraws( p_context );
	}
} // namespace VTX::Renderer::Geometry
