#include "renderer/geometry/ses.hpp"
#include "renderer/binary_buffer.hpp"
#include "renderer/layout/atoms.hpp"
#include "renderer/representation.hpp"
#include <algorithm>
#include <array>
#include <util/chrono.hpp>
#include <util/logger.hpp>
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

		Surface	 surface;
		State	 state				= State::Empty;
		uint32_t atomOffset			= 0;
		uint32_t rendererAtomOffset = 0;
		uint32_t atomNb				= 0;
		uint32_t probeOffset		= 0;
		uint32_t sectorOffset		= 0;
		uint32_t convexPatchNb		= 0;
		uint32_t circlePatchNb		= 0;
		uint32_t segmentPatchNb		= 0;
		uint32_t probeNb			= 0;
		uint32_t sectorNb			= 0;
		uint32_t concavePatchNb		= 0;

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
		constexpr size_t OUTPUT_CONVEX_PATCHES	   = 0;
		constexpr size_t OUTPUT_CIRCLE_PATCHES	   = 1;
		constexpr size_t OUTPUT_SEGMENT_PATCHES	   = 2;
		constexpr size_t OUTPUT_PROBES			   = 3;
		constexpr size_t OUTPUT_PROBE_ATOM_INDICES = 4;
		constexpr size_t OUTPUT_PROBE_NEIGHBORS	   = 5;
		constexpr size_t OUTPUT_SECTORS			   = 6;
		constexpr size_t OUTPUT_BUFFER_NB		   = 7;

		std::array<Desc::BufferRef, OUTPUT_BUFFER_NB> _outputBufferRefs(
			const SES::SurfaceConstruction & p_construction
		)
		{
			const uint32_t chunk = p_construction.surface.id;

			return {
				Desc::BufferRef { SES::BUFFER_CONVEX_PATCH_ELEMENTS, chunk },
				Desc::BufferRef { SES::BUFFER_CIRCLE_PATCH_ATOMS, chunk },
				Desc::BufferRef { SES::BUFFER_SEGMENT_PATCH_IDS, chunk },
				Desc::BufferRef { SES::BUFFER_PROBES },
				Desc::BufferRef { SES::BUFFER_PROBE_ATOM_INDICES },
				Desc::BufferRef { SES::BUFFER_PROBE_NEIGHBORS },
				Desc::BufferRef { SES::BUFFER_SECTORS },
			};
		}

		SESDetail::SesdfOutputBuffers _outputBuffers(
			std::span<const Desc::InteropBufferMapping> p_mappings,
			const SES::SurfaceConstruction &			p_construction
		)
		{
			assert( p_mappings.size() == OUTPUT_BUFFER_NB );

			SESDetail::SesdfOutputBuffers outputBuffers;
			outputBuffers.convexPatches
				= { p_mappings[ OUTPUT_CONVEX_PATCHES ].devicePtr, p_mappings[ OUTPUT_CONVEX_PATCHES ].size, 0 };
			outputBuffers.circlePatches
				= { p_mappings[ OUTPUT_CIRCLE_PATCHES ].devicePtr, p_mappings[ OUTPUT_CIRCLE_PATCHES ].size, 0 };
			outputBuffers.segmentPatches
				= { p_mappings[ OUTPUT_SEGMENT_PATCHES ].devicePtr, p_mappings[ OUTPUT_SEGMENT_PATCHES ].size, 0 };
			outputBuffers.probes		   = { p_mappings[ OUTPUT_PROBES ].devicePtr,
											   p_mappings[ OUTPUT_PROBES ].size,
											   p_construction.probeOffset * sizeof( Vec4f ) };
			outputBuffers.probeAtomIndices = { p_mappings[ OUTPUT_PROBE_ATOM_INDICES ].devicePtr,
											   p_mappings[ OUTPUT_PROBE_ATOM_INDICES ].size,
											   p_construction.probeOffset * sizeof( std::array<int32_t, 4> ) };
			outputBuffers.probeNeighbors
				= { p_mappings[ OUTPUT_PROBE_NEIGHBORS ].devicePtr,
					p_mappings[ OUTPUT_PROBE_NEIGHBORS ].size,
					p_construction.probeOffset * SES::MAX_PROBE_NEIGHBOR_NB * sizeof( Vec4f ) };
			outputBuffers.sectors				  = { p_mappings[ OUTPUT_SECTORS ].devicePtr,
													  p_mappings[ OUTPUT_SECTORS ].size,
													  p_construction.sectorOffset * sizeof( Vec4f ) };
			outputBuffers.atomIndexOffset		  = p_construction.atomOffset;
			outputBuffers.rendererAtomIndexOffset = p_construction.rendererAtomOffset;
			outputBuffers.probeIndexOffset		  = p_construction.probeOffset;
			outputBuffers.sectorIndexOffset		  = p_construction.sectorOffset;
			outputBuffers.maxProbeNeighborNb	  = SES::MAX_PROBE_NEIGHBOR_NB;

			return outputBuffers;
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

		auto construction				 = std::make_unique<SurfaceConstruction>();
		construction->surface			 = _createWholeSurface( p_handle );
		construction->rendererAtomOffset = p_inputAtomOffset;

		for ( const auto & [ surfaceID, existingConstruction ] : _constructions )
		{
			construction->atomOffset += existingConstruction->atomNb;
			construction->probeOffset += existingConstruction->probeNb;
			construction->sectorOffset += existingConstruction->sectorNb;
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
				const std::array<Desc::BufferRef, 4> buffers {
					Desc::BufferRef { Layout::Atoms::ATOMS_POSITIONS },
					Desc::BufferRef { Layout::Atoms::ATOMS_SYMBOLS },
					Desc::BufferRef { BUFFER_ATOMS },
					Desc::BufferRef { BUFFER_ATOM_IDS },
				};

				std::vector<Desc::InteropBufferMapping> mappings;

				try
				{
					mappings = p_context.mapInteropBuffers( Desc::E_INTEROP_API::CUDA, buffers );
					assert( mappings.size() == buffers.size() );

					SESDetail::SesdfInputBuffers inputs;
					inputs.positions  = { mappings[ 0 ].devicePtr, mappings[ 0 ].size, 0 };
					inputs.symbols	  = { mappings[ 1 ].devicePtr, mappings[ 1 ].size, 0 };
					inputs.outputAtoms = { mappings[ 2 ].devicePtr,
										   mappings[ 2 ].size,
										   construction->atomOffset * sizeof( Vec4f ) };
					inputs.outputAtomIds = { mappings[ 3 ].devicePtr,
											 mappings[ 3 ].size,
											 construction->atomOffset * sizeof( uint32_t ) };
					inputs.atomOffset		   = p_inputAtomOffset;
					inputs.rendererAtomOffset = construction->rendererAtomOffset;
					inputs.atomNb			   = uint32_t( atomCount );

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
					construction->state			   = construction->cudaConstruction != nullptr
														 ? SurfaceConstruction::State::PendingWrite
														 : SurfaceConstruction::State::Incalculable;
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

		if ( construction->state == SurfaceConstruction::State::Incalculable )
		{
			_constructEmptyRanges( construction->surface );
		}
		else
		{
			const Surface & surface = construction->surface;
			convexPatches.construct(
				surface.id, surface.system, construction->convexPatchNb, construction->atomOffset
			);
			circlePatches.construct( surface.id, surface.system, construction->circlePatchNb );
			segmentPatches.construct( surface.id, surface.system, construction->segmentPatchNb );
			concavePatches.construct(
				surface.id, surface.system, construction->concavePatchNb, construction->probeOffset
			);
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
		uint32_t probeNb  = 0;
		uint32_t sectorNb = 0;
		for ( const auto & [ surfaceID, construction ] : _constructions )
		{
			probeNb += construction->probeNb;
			sectorNb += construction->sectorNb;
		}

		p_context.setBuffer<Vec4f>( { BUFFER_PROBES }, std::max<uint32_t>( 1u, probeNb ) );
		p_context.setBuffer<std::array<int32_t, 4>>( { BUFFER_PROBE_ATOM_INDICES }, std::max<uint32_t>( 1u, probeNb ) );
		p_context.setBuffer<Vec4f>(
			{ BUFFER_PROBE_NEIGHBORS }, std::max<uint32_t>( 1u, probeNb * MAX_PROBE_NEIGHBOR_NB )
		);
		p_context.setBuffer<Vec4f>( { BUFFER_SECTORS }, std::max<uint32_t>( 1u, sectorNb ) );

		for ( const auto & [ surfaceID, construction ] : _constructions )
		{
			if ( construction->state != SurfaceConstruction::State::PendingWrite
				 && construction->state != SurfaceConstruction::State::Written )
			{
				continue;
			}

			const uint32_t chunk = construction->surface.id;

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

	void SES::uploadIndexes( Context::ContextWrapper & p_context, const Desc::Handle p_handle )
	{
		const auto it = _surfaces.bySystem.find( p_handle );
		assert( it != _surfaces.bySystem.end() );

		for ( const SurfaceID surface : it->second )
		{
			convexPatches.uploadIndexes( p_context, surface );
			circlePatches.uploadIndexes( p_context, surface );
			segmentPatches.uploadIndexes( p_context, surface );
			concavePatches.uploadIndexes( p_context, surface );
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

		std::vector<Desc::InteropBufferMapping> mappings;

		try
		{
			for ( const auto & [ surfaceID, construction ] : _constructions )
			{
				if ( not construction->pendingWrite() )
				{
					continue;
				}

				const std::array<Desc::BufferRef, OUTPUT_BUFFER_NB> buffers = _outputBufferRefs( *construction );
				mappings = p_context.mapInteropBuffers( Desc::E_INTEROP_API::CUDA, buffers );
				assert( mappings.size() == buffers.size() );

				SESDetail::writeCudaConstruction(
					*construction->cudaConstruction, _outputBuffers( mappings, *construction )
				);
				p_context.unmapInteropBuffers( Desc::E_INTEROP_API::CUDA, mappings );
				mappings.clear();

				construction->cudaConstruction.reset();
				construction->state = SurfaceConstruction::State::Written;
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
		return _createSurface( SurfaceKey { p_handle, E_SURFACE_SCOPE::WHOLE, 0 } );
	}

	void SES::_constructEmptyRanges( const Surface & p_surface )
	{
		convexPatches.construct( p_surface.id, p_surface.system, 0 );
		circlePatches.construct( p_surface.id, p_surface.system, 0 );
		segmentPatches.construct( p_surface.id, p_surface.system, 0 );
		concavePatches.construct( p_surface.id, p_surface.system, 0 );
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

	void SES::_discardPendingCompute( Context::ContextWrapper & p_context )
	{
		for ( auto & [ surfaceID, construction ] : _constructions )
		{
			if ( construction->state != SurfaceConstruction::State::PendingWrite )
			{
				continue;
			}

			_disableDraws( p_context, surfaceID );
			construction->markIncalculable();
		}
	}
} // namespace VTX::Renderer::Geometry
