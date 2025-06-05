#ifndef BCS_CUDA_EXECUTION_CUH
#define BCS_CUDA_EXECUTION_CUH

namespace bcs
{
    template<uint32_t WarpThreadNb = 32>
    __device__ uint32_t getIdFromMask( uint32_t mask )
    {
        return ( WarpThreadNb - 1u ) - __clz( mask );
    }

    // Use the mask as a processing queue. Every 1 in the mask's bit representation
    // will produce a call to predicate with its corresponding index in the mask.
    template<uint32_t WarpThreadNb, typename Predicate>
    __device__ void executeQueue( uint32_t mask, Predicate predicate )
    {
        while ( mask )
        {
            const uint32_t id = getIdFromMask( mask );
            predicate( id );
            mask &= ~( 1u << id );
        }
    }

    // Reference:
    // https://github.com/rapidsai/cudf/blob/89b802e6cecffe2425048f1f70cd682b865730b8/cpp/include/cudf/detail/utilities/device_atomics.cuh#L323
    // Found: https://github.com/owensgroup/RXMesh/blob/main/include/rxmesh/kernels/util.cuh#L94
    __forceinline__ __device__ uint8_t atomicCAS( uint8_t *       addr,
                                                  uint8_t const & compare,
                                                  uint8_t const & update_value )
    {
        using T_int = unsigned int;

        T_int   shift          = ( ( reinterpret_cast<size_t>( addr ) & 3 ) * 8 );
        T_int * address_uint32 = reinterpret_cast<T_int *>( addr - ( reinterpret_cast<size_t>( addr ) & 3 ) );

        // the 'target_value' in `old` can be different from `compare`
        // because other thread may update the value
        // before fetching a value from `address_uint32` in this function
        T_int   old = *address_uint32;
        T_int   assumed;
        uint8_t target_value;
        uint8_t u_val = *reinterpret_cast<const uint8_t *>( &update_value );

        do
        {
            assumed      = old;
            target_value = uint8_t( ( old >> shift ) & 0xff );
            // have to compare `target_value` and `compare` before calling atomicCAS
            // the `target_value` in `old` can be different with `compare`
            if ( target_value != compare )
                break;

            T_int new_value = ( old & ~( 0x000000ff << shift ) ) | ( T_int( u_val ) << shift );
            old             = ::atomicCAS( address_uint32, assumed, new_value );
        } while ( assumed != old );

        return target_value;
    }
} // namespace bcs

#endif // BCS_CUDA_EXECUTION_CUH