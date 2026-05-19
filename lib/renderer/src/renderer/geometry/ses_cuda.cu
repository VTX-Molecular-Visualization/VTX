#include "bcs/cuda/memory.cuh"
#include "bcs/sesdf/sesdf.hpp"
#include "renderer/geometry/ses_cuda.hpp"
#include <glm/common.hpp>
#include <stdexcept>

namespace VTX::Renderer::Geometry::SESDetail
{
	struct CudaConstruction
	{
		std::vector<bcs::Vec4f>		molecule;
		std::unique_ptr<bcs::Sesdf> ses;
		uint32_t					sectorNb = 0;
	};

	void CudaConstructionDeleter::operator()( CudaConstruction * p_construction ) const { delete p_construction; }

	namespace
	{
		bcs::Aabb _computeAabb( const std::span<const Vec3f> p_positions )
		{
			bcs::Aabb aabb { p_positions.front(), p_positions.front() };

			for ( const Vec3f & position : p_positions )
			{
				aabb.min = glm::min( aabb.min, position );
				aabb.max = glm::max( aabb.max, position );
			}

			return aabb;
		}

		uint32_t _getActualSectorNb( const bcs::sesdf::SesdfData & p_data )
		{
			if ( p_data.convexPatchNb == 0 || p_data.convexPatches == nullptr )
			{
				return 0;
			}

			uint2			  lastRange {};
			const cudaError_t error = cudaMemcpy(
				&lastRange, p_data.convexPatches + p_data.convexPatchNb - 1, sizeof( uint2 ), cudaMemcpyDeviceToHost
			);
			if ( error != cudaSuccess )
			{
				throw std::runtime_error(
					std::string( "Can not build SES: sector count read failed: " ) + cudaGetErrorString( error )
				);
			}

			return lastRange.y;
		}

		void _checkTarget( const CudaBufferView & p_target, const size_t p_size, const char * const p_name )
		{
			if ( p_size == 0 )
			{
				return;
			}

			if ( p_target.devicePtr == nullptr )
			{
				throw std::runtime_error(
					std::string( "Can not write SES: target CUDA buffer is not mapped for " ) + p_name
				);
			}

			if ( p_target.sizeBytes < p_target.offsetBytes + p_size )
			{
				throw std::runtime_error(
					std::string( "Can not write SES: target CUDA buffer is too small for " ) + p_name
				);
			}
		}

		template<typename T>
		T * _targetPtr( const CudaBufferView & p_target, const size_t p_size, const char * const p_name )
		{
			if ( p_size == 0 )
			{
				return nullptr;
			}

			_checkTarget( p_target, p_size, p_name );
			return reinterpret_cast<T *>( static_cast<uint8_t *>( p_target.devicePtr ) + p_target.offsetBytes );
		}

		void _cudaCheck( const cudaError_t p_error, const char * const p_context )
		{
			if ( p_error == cudaSuccess )
			{
				return;
			}

			throw std::runtime_error( std::string( p_context ) + ": " + cudaGetErrorString( p_error ) );
		}

		__global__ void _packAtomsKernel(
			float4 *	   p_dst,
			const float *  p_positions,
			const uint8_t * p_symbols,
			const float *  p_vdwRadii,
			const uint32_t p_atomNb
		)
		{
			const uint32_t atomIndex = blockIdx.x * blockDim.x + threadIdx.x;
			if ( atomIndex >= p_atomNb )
			{
				return;
			}

			const uint32_t positionOffset = atomIndex * 3u;
			const uint8_t  symbol		 = p_symbols[ atomIndex ];
			p_dst[ atomIndex ]			 = make_float4(
					   p_positions[ positionOffset ],
					   p_positions[ positionOffset + 1u ],
					   p_positions[ positionOffset + 2u ],
					   p_vdwRadii[ symbol ]
				   );
		}

		void _packRendererAtoms( const SesdfInputBuffers & p_inputs, float4 * const p_dst )
		{
			constexpr uint32_t threadNb = 256u;
			const uint32_t	  blockNb  = ( p_inputs.atomNb + threadNb - 1u ) / threadNb;

			bcs::DeviceBuffer dVdwRadii = bcs::DeviceBuffer::Typed<float>(
				Core::ChemDB::Atom::SYMBOL_VDW_RADIUS.size()
			);
			_cudaCheck(
				cudaMemcpy(
					dVdwRadii.get<float>(),
					Core::ChemDB::Atom::SYMBOL_VDW_RADIUS.data(),
					Core::ChemDB::Atom::SYMBOL_VDW_RADIUS.size() * sizeof( float ),
					cudaMemcpyHostToDevice
				),
				"SES VDW radius upload failed"
			);

			const auto * positions = reinterpret_cast<const float *>(
				static_cast<const uint8_t *>( p_inputs.positions.devicePtr )
				+ p_inputs.positions.offsetBytes
				+ p_inputs.atomOffset * sizeof( Vec3f )
			);
			const auto * symbols = reinterpret_cast<const uint8_t *>(
				static_cast<const uint8_t *>( p_inputs.symbols.devicePtr )
				+ p_inputs.symbols.offsetBytes
				+ p_inputs.atomOffset * sizeof( Core::ChemDB::Atom::SYMBOL )
			);

			_packAtomsKernel<<<blockNb, threadNb>>>(
				p_dst, positions, symbols, dVdwRadii.get<float>(), p_inputs.atomNb
			);
			_cudaCheck( cudaGetLastError(), "SES atom input packing failed" );
			_cudaCheck( cudaDeviceSynchronize(), "SES atom input packing synchronization failed" );
		}
	} // namespace

	CudaBuildResult buildCudaConstruction(
		const std::span<const Vec3f>					  p_positions,
		const std::span<const Core::ChemDB::Atom::SYMBOL> p_symbols,
		const float										  p_probeRadius
	)
	{
		CudaBuildResult result;

		const size_t atomCount = p_positions.size();
		if ( atomCount == 0 )
		{
			return result;
		}

		auto construction = std::make_unique<CudaConstruction>();
		construction->molecule.reserve( atomCount );

		for ( size_t atomIndex = 0; atomIndex < atomCount; ++atomIndex )
		{
			const Vec3f & position = p_positions[ atomIndex ];
			const auto	  symbol   = p_symbols[ atomIndex ];
			const float	  radius   = Core::ChemDB::Atom::SYMBOL_VDW_RADIUS[ size_t( symbol ) ];

			construction->molecule.emplace_back( position, radius );
		}

		const bcs::Aabb aabb = _computeAabb( p_positions );
		construction->ses	 = std::make_unique<bcs::Sesdf>(
			   bcs::ConstSpan<bcs::Vec4f>( construction->molecule ), aabb, p_probeRadius, true, false
		   );

		const bcs::sesdf::SesdfData data = construction->ses->getData();
		construction->sectorNb			 = _getActualSectorNb( data );

		result.atomNb		  = data.atomNb;
		result.convexPatchNb  = data.convexPatchNb;
		result.circlePatchNb  = data.circlePatchNb;
		result.segmentPatchNb = data.segmentPatchNb;
		result.probeNb		  = data.concavePatchNb;
		result.sectorNb		  = construction->sectorNb;
		result.concavePatchNb = data.concavePatchNb;

		result.construction = CudaConstructionPtr( construction.release() );

		return result;
	}

	CudaBuildResult buildCudaConstructionFromRendererBuffers(
		const SesdfInputBuffers & p_inputs,
		const std::span<const Vec3f> p_aabbPositions,
		const float					 p_probeRadius
	)
	{
		CudaBuildResult result;

		if ( p_inputs.atomNb == 0 )
		{
			return result;
		}

		if ( p_inputs.positions.devicePtr == nullptr || p_inputs.symbols.devicePtr == nullptr )
		{
			throw std::runtime_error( "Can not build SES: renderer atom input buffers are not mapped." );
		}

		const size_t positionSize = ( p_inputs.atomOffset + p_inputs.atomNb ) * sizeof( Vec3f );
		const size_t symbolSize
			= ( p_inputs.atomOffset + p_inputs.atomNb ) * sizeof( Core::ChemDB::Atom::SYMBOL );
		if ( p_inputs.positions.sizeBytes < positionSize || p_inputs.symbols.sizeBytes < symbolSize )
		{
			throw std::runtime_error( "Can not build SES: renderer atom input buffers are too small." );
		}

		auto construction = std::make_unique<CudaConstruction>();

		const bcs::Aabb aabb = _computeAabb( p_aabbPositions );
		construction->ses	 = std::make_unique<bcs::Sesdf>(
			   bcs::ConstSpan<bcs::Vec4f>( nullptr, p_inputs.atomNb ),
			   aabb,
			   p_probeRadius,
			   false,
			   false
		   );
		_packRendererAtoms( p_inputs, construction->ses->getDAtoms() );
		construction->ses->build();

		const bcs::sesdf::SesdfData data = construction->ses->getData();
		construction->sectorNb			 = _getActualSectorNb( data );

		result.atomNb		  = data.atomNb;
		result.convexPatchNb  = data.convexPatchNb;
		result.circlePatchNb  = data.circlePatchNb;
		result.segmentPatchNb = data.segmentPatchNb;
		result.probeNb		  = data.concavePatchNb;
		result.sectorNb		  = construction->sectorNb;
		result.concavePatchNb = data.concavePatchNb;

		result.construction = CudaConstructionPtr( construction.release() );

		return result;
	}

	void writeCudaConstruction( CudaConstruction & p_construction, const SesdfRenderBuffers & p_targets )
	{
		bcs::sesdf::SesdfData data = p_construction.ses->getData();

		if ( p_targets.maxProbeNeighborNb < data.maxConcaveNeighbors )
		{
			throw std::runtime_error( "Can not write SES: target probe neighbor stride is too small." );
		}

		bcs::sesdf::SesdfWriteBuffers output;
		output.atoms = _targetPtr<float4>( p_targets.atoms, data.atomNb * sizeof( float4 ), "Atoms" );
		output.convexPatches
			= _targetPtr<uint2>( p_targets.convexPatches, data.convexPatchNb * sizeof( uint2 ), "ConvexPatches" );
		output.circlePatches
			= _targetPtr<uint2>( p_targets.circlePatches, data.circlePatchNb * sizeof( uint2 ), "CirclePatches" );
		output.segmentPatches
			= _targetPtr<uint4>( p_targets.segmentPatches, data.segmentPatchNb * sizeof( uint4 ), "SegmentPatches" );
		output.concavePatchesPosition
			= _targetPtr<float4>( p_targets.probes, data.concavePatchNb * sizeof( float4 ), "Probes" );
		output.concavePatchesId
			= _targetPtr<int4>( p_targets.probeAtomIndices, data.concavePatchNb * sizeof( int4 ), "ProbeAtomIndices" );
		output.concavePatchesNeighbors = _targetPtr<float4>(
			p_targets.probeNeighbors,
			data.concavePatchNb * p_targets.maxProbeNeighborNb * sizeof( float4 ),
			"ProbeNeighbors"
		);
		output.sectors = _targetPtr<float4>( p_targets.sectors, p_construction.sectorNb * sizeof( float4 ), "Sectors" );

		output.atomIndexOffset	   = p_targets.atomIndexOffset;
		output.probeIndexOffset	   = p_targets.probeIndexOffset;
		output.sectorIndexOffset   = p_targets.sectorIndexOffset;
		output.maxConcaveNeighbors = p_targets.maxProbeNeighborNb;

		p_construction.ses->writeData( output );
	}
} // namespace VTX::Renderer::Geometry::SESDetail
