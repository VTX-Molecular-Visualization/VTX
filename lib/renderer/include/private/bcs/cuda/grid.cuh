#ifndef BCS_CUDA_GRID_CUH
#define BCS_CUDA_GRID_CUH

#include "bcs/cuda/memory.cuh"

namespace bcs
{
    struct GridInfo
    {
        float3 worldOrigin;
        float3 cellSize;
        int3   size;

        __host__ __device__ int3     getGridPosition( const float3 & position ) const;
        __host__ __device__ uint32_t getHash( const float3 & position ) const;
        __host__ __device__ uint32_t getHash( const int3 position ) const;
    };

    __global__ void computeHashes( const GridInfo       gridInfo,
                                   const uint32_t       elementNb,
                                   const float4 * const positions,
                                   uint32_t *           hashes,
                                   uint32_t *           indices );
    __global__ void buildAccelerationGrid( const uint32_t         elementNb,
                                           const uint32_t * const hashes,
                                           const uint32_t * const indices,
                                           const float4 * const   oldPos,
                                           uint32_t *             cellStarts,
                                           uint32_t *             cellEnds,
                                           float4 *               sortedPos );

    class AccelerationGrid
    {
      public:
        static constexpr uint32_t EmptyGridCellValue = 0xffffffff;

        AccelerationGrid() = default;
        AccelerationGrid( GridInfo configuration, cudaStream_t stream = 0 );

        AccelerationGrid( const AccelerationGrid & ) = delete;
        AccelerationGrid & operator=( const AccelerationGrid & ) = delete;

        AccelerationGrid( AccelerationGrid && ) = default;
        AccelerationGrid & operator=( AccelerationGrid && ) = default;

        ~AccelerationGrid() = default;

        void build( const uint32_t elementNb, const float4 * const positions );

        inline const GridInfo & getConfiguration() const;
        inline const uint32_t * getCellsStart() const;
        inline const uint32_t * getCellsEnd() const;
        inline const uint32_t * getSortedIndices() const;
        inline const uint32_t * getSortedIndices();
        inline const float4 *   getSortedPosition() const;

        struct Cells
        {
            const uint32_t * starts;
            const uint32_t * ends;
        };
        inline Cells getCells() const;

      private:
        GridInfo m_configuration {};
        uint32_t m_cellNb = 0;

        DeviceBuffer m_dSortedPositions;
        DeviceBuffer m_dHashes;
        DeviceBuffer m_dIndices;
        DeviceBuffer m_dCellsStart;
        DeviceBuffer m_dCellsEnd;
    };
} // namespace bcs

#include "bcs/cuda/grid.inl"

#endif // BCS_CUDA_GRID_CUH
