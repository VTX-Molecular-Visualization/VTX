#include "bcs/cuda/memory.cuh"
#include "bcs/sesdf/sesdf.hpp"
#include "renderer/geometry/ses_cuda.hpp"
#include <glm/common.hpp>
#include <algorithm>
#include <stdexcept>
#include <vector>

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

		bcs::Aabb _computeAabb( const std::span<const Vec3f> p_positions, const std::span<const Index> p_atomIndices )
		{
			bcs::Aabb aabb { p_positions[ p_atomIndices.front() ], p_positions[ p_atomIndices.front() ] };

			for ( const Index atom : p_atomIndices )
			{
				const Vec3f & position = p_positions[ atom ];
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

		template<typename T>
		T * _optionalTargetPtr( const CudaBufferView & p_target, const size_t p_size, const char * const p_name )
		{
			if ( p_target.devicePtr == nullptr )
			{
				return nullptr;
			}

			return _targetPtr<T>( p_target, p_size, p_name );
		}

		void _cudaCheck( const cudaError_t p_error, const char * const p_context )
		{
			if ( p_error == cudaSuccess )
			{
				return;
			}

			throw std::runtime_error( std::string( p_context ) + ": " + cudaGetErrorString( p_error ) );
		}

		template<typename T>
		std::vector<T> _copyDeviceBuffer( const T * const p_src, const uint32_t p_count, const char * const p_context )
		{
			std::vector<T> res( p_count );
			if ( p_count == 0 )
			{
				return res;
			}

			_cudaCheck(
				cudaMemcpy( res.data(), p_src, size_t( p_count ) * sizeof( T ), cudaMemcpyDeviceToHost ),
				p_context
			);
			return res;
		}

		float4 * _optionalTargetAtoms( const SesdfInputBuffers & p_inputs )
		{
			if ( p_inputs.outputAtoms.devicePtr == nullptr )
			{
				return nullptr;
			}

			const size_t atomSize = p_inputs.atomNb * sizeof( float4 );
			_checkTarget( p_inputs.outputAtoms, atomSize, "OutputAtoms" );
			return reinterpret_cast<float4 *>(
				static_cast<uint8_t *>( p_inputs.outputAtoms.devicePtr ) + p_inputs.outputAtoms.offsetBytes
			);
		}

		uint32_t * _optionalTargetAtomIds( const SesdfInputBuffers & p_inputs )
		{
			if ( p_inputs.outputAtomIds.devicePtr == nullptr )
			{
				return nullptr;
			}

			const size_t atomIdSize = p_inputs.atomNb * sizeof( uint32_t );
			_checkTarget( p_inputs.outputAtomIds, atomIdSize, "OutputAtomIds" );
			return reinterpret_cast<uint32_t *>(
				static_cast<uint8_t *>( p_inputs.outputAtomIds.devicePtr ) + p_inputs.outputAtomIds.offsetBytes
			);
		}

		__global__ void _packAtomsKernel(
			float4 *		p_dst,
			const float *	p_positions,
			const uint8_t * p_symbols,
			const float *	p_vdwRadii,
			const uint32_t	p_atomNb
		)
		{
			const uint32_t atomIndex = blockIdx.x * blockDim.x + threadIdx.x;
			if ( atomIndex >= p_atomNb )
			{
				return;
			}

			const uint32_t positionOffset = atomIndex * 3u;
			const uint8_t  symbol		  = p_symbols[ atomIndex ];
			p_dst[ atomIndex ]			  = make_float4(
				   p_positions[ positionOffset ],
				   p_positions[ positionOffset + 1u ],
				   p_positions[ positionOffset + 2u ],
				   p_vdwRadii[ symbol ]
			   );
		}

		__global__ void _packIndexedAtomsKernel(
			float4 *		 p_dst,
			const float *	 p_positions,
			const uint8_t *	 p_symbols,
			const uint32_t * p_atomIndices,
			const float *	 p_vdwRadii,
			const uint32_t	 p_atomOffset,
			const uint32_t	 p_atomNb
		)
		{
			const uint32_t localAtomIndex = blockIdx.x * blockDim.x + threadIdx.x;
			if ( localAtomIndex >= p_atomNb )
			{
				return;
			}

			const uint32_t atomIndex		= p_atomOffset + p_atomIndices[ localAtomIndex ];
			const uint32_t positionOffset = atomIndex * 3u;
			const uint8_t  symbol		= p_symbols[ atomIndex ];
			p_dst[ localAtomIndex ]		= make_float4(
				 p_positions[ positionOffset ],
				 p_positions[ positionOffset + 1u ],
				 p_positions[ positionOffset + 2u ],
				 p_vdwRadii[ symbol ]
			 );
		}

		__global__ void _writeIndexedAtomIdsKernel(
			uint32_t *		 p_dst,
			const uint32_t * p_sortedLocalAtomIds,
			const uint32_t * p_atomIndices,
			const uint32_t	 p_rendererAtomOffset,
			const uint32_t	 p_atomNb
		)
		{
			const uint32_t atomIndex = blockIdx.x * blockDim.x + threadIdx.x;
			if ( atomIndex >= p_atomNb )
			{
				return;
			}

			p_dst[ atomIndex ] = p_rendererAtomOffset + p_atomIndices[ p_sortedLocalAtomIds[ atomIndex ] ];
		}

		bcs::DeviceBuffer _uploadAtomIndices( const std::span<const Index> p_atomIndices )
		{
			std::vector<uint32_t> indices;
			indices.reserve( p_atomIndices.size() );
			for ( const Index atom : p_atomIndices )
			{
				indices.emplace_back( uint32_t( atom ) );
			}

			bcs::DeviceBuffer dAtomIndices = bcs::DeviceBuffer::Typed<uint32_t>( indices.size() );
			_cudaCheck(
				cudaMemcpy(
					dAtomIndices.get<uint32_t>(),
					indices.data(),
					indices.size() * sizeof( uint32_t ),
					cudaMemcpyHostToDevice
				),
				"SES indexed atom upload failed"
			);

			return dAtomIndices;
		}

		void _packRendererAtoms( const SesdfInputBuffers & p_inputs, float4 * const p_dst )
		{
			constexpr uint32_t threadNb = 256u;
			const uint32_t	   blockNb	= ( p_inputs.atomNb + threadNb - 1u ) / threadNb;

			bcs::DeviceBuffer dVdwRadii
				= bcs::DeviceBuffer::Typed<float>( Core::ChemDB::Atom::SYMBOL_VDW_RADIUS.size() );
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
				static_cast<const uint8_t *>( p_inputs.positions.devicePtr ) + p_inputs.positions.offsetBytes
				+ p_inputs.atomOffset * sizeof( Vec3f )
			);
			const auto * symbols = reinterpret_cast<const uint8_t *>(
				static_cast<const uint8_t *>( p_inputs.symbols.devicePtr ) + p_inputs.symbols.offsetBytes
				+ p_inputs.atomOffset * sizeof( Core::ChemDB::Atom::SYMBOL )
			);

			_packAtomsKernel<<<blockNb, threadNb>>>(
				p_dst, positions, symbols, dVdwRadii.get<float>(), p_inputs.atomNb
			);
			_cudaCheck( cudaGetLastError(), "SES atom input packing failed" );
			_cudaCheck( cudaDeviceSynchronize(), "SES atom input packing synchronization failed" );
		}

		void _packRendererAtoms(
			const SesdfInputBuffers & p_inputs,
			const bcs::DeviceBuffer & p_atomIndices,
			float4 * const			  p_dst
		)
		{
			constexpr uint32_t threadNb = 256u;
			const uint32_t	   blockNb	= ( p_inputs.atomNb + threadNb - 1u ) / threadNb;

			bcs::DeviceBuffer dVdwRadii
				= bcs::DeviceBuffer::Typed<float>( Core::ChemDB::Atom::SYMBOL_VDW_RADIUS.size() );
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
				static_cast<const uint8_t *>( p_inputs.positions.devicePtr ) + p_inputs.positions.offsetBytes
			);
			const auto * symbols = reinterpret_cast<const uint8_t *>(
				static_cast<const uint8_t *>( p_inputs.symbols.devicePtr ) + p_inputs.symbols.offsetBytes
			);

			_packIndexedAtomsKernel<<<blockNb, threadNb>>>(
				p_dst,
				positions,
				symbols,
				p_atomIndices.get<uint32_t>(),
				dVdwRadii.get<float>(),
				p_inputs.atomOffset,
				p_inputs.atomNb
			);
			_cudaCheck( cudaGetLastError(), "SES indexed atom input packing failed" );
			_cudaCheck( cudaDeviceSynchronize(), "SES indexed atom input packing synchronization failed" );
		}

		void _writeIndexedAtomIds(
			const SesdfInputBuffers & p_inputs,
			const bcs::DeviceBuffer & p_atomIndices,
			CudaConstruction &		 p_construction
		)
		{
			uint32_t * const outputAtomIds = _optionalTargetAtomIds( p_inputs );
			if ( outputAtomIds == nullptr )
			{
				return;
			}

			constexpr uint32_t threadNb = 256u;
			const uint32_t	   blockNb	= ( p_inputs.atomNb + threadNb - 1u ) / threadNb;
			_writeIndexedAtomIdsKernel<<<blockNb, threadNb>>>(
				outputAtomIds,
				p_construction.ses->getDAtomIndices().get<uint32_t>(),
				p_atomIndices.get<uint32_t>(),
				p_inputs.rendererAtomOffset,
				p_inputs.atomNb
			);
			_cudaCheck( cudaGetLastError(), "SES indexed atom id write failed" );
			_cudaCheck( cudaDeviceSynchronize(), "SES indexed atom id write synchronization failed" );
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
		const SesdfInputBuffers &	 p_inputs,
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
		const size_t symbolSize	  = ( p_inputs.atomOffset + p_inputs.atomNb ) * sizeof( Core::ChemDB::Atom::SYMBOL );
		if ( p_inputs.positions.sizeBytes < positionSize || p_inputs.symbols.sizeBytes < symbolSize )
		{
			throw std::runtime_error( "Can not build SES: renderer atom input buffers are too small." );
		}

		auto construction = std::make_unique<CudaConstruction>();

		const bcs::Aabb	 aabb		   = _computeAabb( p_aabbPositions );
		float4 * const	 outputAtoms   = _optionalTargetAtoms( p_inputs );
		uint32_t * const outputAtomIds = _optionalTargetAtomIds( p_inputs );
		construction->ses			   = std::make_unique<bcs::Sesdf>(
			 bcs::ConstSpan<bcs::Vec4f>( nullptr, p_inputs.atomNb ),
			 aabb,
			 p_probeRadius,
			 false,
			 false,
			 outputAtoms,
			 outputAtomIds,
			 p_inputs.rendererAtomOffset
		);
		_packRendererAtoms( p_inputs, construction->ses->getDAtoms() );
		construction->ses->build();
		_cudaCheck( cudaGetLastError(), "SES construction failed" );
		_cudaCheck( cudaDeviceSynchronize(), "SES construction synchronization failed" );

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
		const SesdfInputBuffers &	 p_inputs,
		const std::span<const Vec3f> p_aabbPositions,
		const std::span<const Index> p_atomIndices,
		const float					 p_probeRadius
	)
	{
		CudaBuildResult result;

		if ( p_atomIndices.empty() )
		{
			return result;
		}

		if ( p_inputs.positions.devicePtr == nullptr || p_inputs.symbols.devicePtr == nullptr )
		{
			throw std::runtime_error( "Can not build SES: renderer atom input buffers are not mapped." );
		}

		Index maxAtomIndex = 0;
		for ( const Index atom : p_atomIndices )
		{
			maxAtomIndex = std::max( maxAtomIndex, atom );
		}

		const size_t positionSize = ( p_inputs.atomOffset + maxAtomIndex + 1u ) * sizeof( Vec3f );
		const size_t symbolSize
			= ( p_inputs.atomOffset + maxAtomIndex + 1u ) * sizeof( Core::ChemDB::Atom::SYMBOL );
		if ( p_inputs.positions.sizeBytes < positionSize || p_inputs.symbols.sizeBytes < symbolSize )
		{
			throw std::runtime_error( "Can not build SES: renderer atom input buffers are too small." );
		}

		auto construction = std::make_unique<CudaConstruction>();

		const bcs::Aabb	 aabb		   = _computeAabb( p_aabbPositions, p_atomIndices );
		float4 * const	 outputAtoms   = _optionalTargetAtoms( p_inputs );
		uint32_t * const outputAtomIds = _optionalTargetAtomIds( p_inputs );
		construction->ses			   = std::make_unique<bcs::Sesdf>(
			 bcs::ConstSpan<bcs::Vec4f>( nullptr, p_inputs.atomNb ),
			 aabb,
			 p_probeRadius,
			 false,
			 false,
			 outputAtoms,
			 outputAtomIds,
			 0
		);

		bcs::DeviceBuffer atomIndices = _uploadAtomIndices( p_atomIndices );
		_packRendererAtoms( p_inputs, atomIndices, construction->ses->getDAtoms() );
		construction->ses->build();
		_cudaCheck( cudaGetLastError(), "SES construction failed" );
		_cudaCheck( cudaDeviceSynchronize(), "SES construction synchronization failed" );
		_writeIndexedAtomIds( p_inputs, atomIndices, *construction );

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

	void writeCudaConstruction( CudaConstruction & p_construction, const SesdfOutputBuffers & p_targets )
	{
		bcs::sesdf::SesdfData data = p_construction.ses->getData();

		if ( p_targets.maxProbeNeighborNb < data.maxConcaveNeighbors )
		{
			throw std::runtime_error( "Can not write SES: target probe neighbor stride is too small." );
		}

		bcs::sesdf::SesdfWriteBuffers output;
		output.atoms   = _optionalTargetPtr<float4>( p_targets.atoms, data.atomNb * sizeof( float4 ), "Atoms" );
		output.atomIds = _optionalTargetPtr<uint32_t>( p_targets.atomIds, data.atomNb * sizeof( uint32_t ), "AtomIds" );
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
		output.atomIdOffset		   = p_targets.rendererAtomIndexOffset;
		output.probeIndexOffset	   = p_targets.probeIndexOffset;
		output.sectorIndexOffset   = p_targets.sectorIndexOffset;
		output.maxConcaveNeighbors = p_targets.maxProbeNeighborNb;

		p_construction.ses->writeData( output );
		_cudaCheck( cudaGetLastError(), "SES renderer buffer write failed" );
		_cudaCheck( cudaDeviceSynchronize(), "SES renderer buffer write synchronization failed" );
	}

	std::vector<uint32_t> readAtomIds( const CudaBufferView & p_buffer, const uint32_t p_count )
	{
		const uint32_t * const atomIds = _targetPtr<uint32_t>( p_buffer, p_count * sizeof( uint32_t ), "AtomIds" );
		return _copyDeviceBuffer( atomIds, p_count, "SES atom id read failed" );
	}

	void readConstructionVisibilityData( CudaConstruction & p_construction, SesdfVisibilityData & p_visibility )
	{
		bcs::sesdf::SesdfData data = p_construction.ses->getData();

		p_visibility.circleAtoms.clear();
		p_visibility.segmentAtoms.clear();
		p_visibility.concaveAtoms.clear();

		if ( data.circlePatchNb > 0 )
		{
			const std::vector<uint2> circleAtoms
				= _copyDeviceBuffer( data.circlePatches, data.circlePatchNb, "SES circle visibility read failed" );
			p_visibility.circleAtoms.resize( circleAtoms.size() );
			for ( size_t i = 0; i < circleAtoms.size(); ++i )
			{
				p_visibility.circleAtoms[ i ] = UVec2 { circleAtoms[ i ].x, circleAtoms[ i ].y };
			}
		}

		if ( data.segmentPatchNb > 0 )
		{
			const std::vector<uint4> segmentAtoms
				= _copyDeviceBuffer( data.segmentPatches, data.segmentPatchNb, "SES segment visibility read failed" );
			p_visibility.segmentAtoms.resize( segmentAtoms.size() );
			for ( size_t i = 0; i < segmentAtoms.size(); ++i )
			{
				p_visibility.segmentAtoms[ i ] = UVec2 { segmentAtoms[ i ].x, segmentAtoms[ i ].y };
			}
		}

		if ( data.concavePatchNb > 0 )
		{
			const std::vector<int4> concaveAtoms
				= _copyDeviceBuffer( data.concavePatchesId, data.concavePatchNb, "SES concave visibility read failed" );
			p_visibility.concaveAtoms.resize( concaveAtoms.size() );
			for ( size_t i = 0; i < concaveAtoms.size(); ++i )
			{
				p_visibility.concaveAtoms[ i ] = IVec4 {
					concaveAtoms[ i ].x, concaveAtoms[ i ].y, concaveAtoms[ i ].z, concaveAtoms[ i ].w
				};
			}
		}
	}

	void releaseCachedCudaMemory()
	{
		int deviceId = 0;
		if ( cudaGetDevice( &deviceId ) != cudaSuccess )
		{
			return;
		}

		cudaMemPool_t memPool {};
		if ( cudaDeviceGetDefaultMemPool( &memPool, deviceId ) != cudaSuccess )
		{
			return;
		}
		if ( cudaDeviceSynchronize() != cudaSuccess )
		{
			return;
		}

		cudaMemPoolTrimTo( memPool, 0 );
	}
} // namespace VTX::Renderer::Geometry::SESDetail
