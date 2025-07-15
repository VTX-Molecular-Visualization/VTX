#include "bcs/cuda/grid.cuh"

namespace bcs
{
	// Based on NVIDIA particle simulation samples
	// Reference:
	// https://github.com/NVIDIA/cuda-samples/blob/master/Samples/2_Concepts_and_Techniques/particles/particles_kernel_impl.cuh
	__host__ __device__ inline int3 GridInfo::getGridPosition( const float3 & position ) const
	{
		int3 gridPos;
		gridPos.x = static_cast<int>( floor( ( position.x - worldOrigin.x ) / cellSize.x ) );
		gridPos.y = static_cast<int>( floor( ( position.y - worldOrigin.y ) / cellSize.y ) );
		gridPos.z = static_cast<int>( floor( ( position.z - worldOrigin.z ) / cellSize.z ) );
		return gridPos;
	}

	__host__ __device__ inline uint32_t GridInfo::getHash( const float3 & position ) const
	{
		return getHash( getGridPosition( position ) );
	}

	__host__ __device__ inline uint32_t GridInfo::getHash( int3 gridPos ) const
	{
		gridPos.x = gridPos.x & ( size.x - 1 ); // wrap grid, assumes size is power of 2
		gridPos.y = gridPos.y & ( size.y - 1 );
		gridPos.z = gridPos.z & ( size.z - 1 );
		return gridPos.z * size.y * size.x + gridPos.y * size.x + gridPos.x;
	}

	inline const GridInfo & AccelerationGrid::getConfiguration() const { return m_configuration; }
	inline const uint32_t * AccelerationGrid::getCellsStart() const { return m_dCellsStart.get<uint32_t>(); }
	inline const uint32_t * AccelerationGrid::getCellsEnd() const { return m_dCellsEnd.get<uint32_t>(); }
	inline const uint32_t * AccelerationGrid::getSortedIndices() const { return m_dIndices.get<uint32_t>(); }
	inline const uint32_t * AccelerationGrid::getSortedIndices() { return m_dIndices.get<uint32_t>(); }
	inline const float4 *	AccelerationGrid::getSortedPosition() const { return m_dSortedPositions.get<float4>(); }
	inline AccelerationGrid::Cells AccelerationGrid::getCells() const { return { getCellsStart(), getCellsEnd() }; }
} // namespace bcs
