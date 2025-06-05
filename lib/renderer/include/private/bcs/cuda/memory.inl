#include <thrust/device_ptr.h>
#include <thrust/transform_scan.h>

#include "bcs/cuda/memory.cuh"

namespace bcs
{
#ifdef __CUDACC__
    template<class Type>
    __global__ void copyImpl( Type * const __restrict__ dst, const Type * const __restrict__ src, std::size_t count )
    {
        for ( int i = blockIdx.x * blockDim.x + threadIdx.x; i < count; i += blockDim.x * gridDim.x )
        {
            dst[ i ] = src[ i ];
        }
    }

    template<class Type>
    void copy( Type * const __restrict__ dst, const Type * const __restrict__ src, uint32_t count )
    {
        auto [ numBlocks, numThreads ] = KernelConfig::From( count, 256 );
        copyImpl<<<numBlocks, numThreads>>>( dst, src, count );
        cudaCheck( "Device to Device copy failed" );
    }

    template<class Type1, class Type2>
    __host__ void prefixSumCount( Type1 * src, Type2 * dst, Type2 * countValue, uint32_t size )
    {
        if constexpr ( std::is_same_v<Type1, Type2> )
        {
            thrust::exclusive_scan( thrust::device,
                                    thrust::device_ptr<Type1>( src ),
                                    thrust::device_ptr<Type1>( src + size ),
                                    thrust::device_ptr<Type1>( dst ) );
        }
        else
        {
            thrust::transform_exclusive_scan(
                thrust::device,
                thrust::device_ptr<Type1>( src ),
                thrust::device_ptr<Type1>( src + size ),
                thrust::device_ptr<Type2>( dst ),
                [] __device__( Type1 c ) { return static_cast<Type2>( c ); },
                0,
                thrust::plus<Type2>() );
        }

        mmemcpy<MemcpyType::DeviceToHost>( countValue, dst + size - 1, 1 );
    }
#endif // __CUDACC__

    template<class Type>
    GraphicsDeviceBuffer GraphicsDeviceBuffer::Typed( const std::size_t count, AccessType access, bool zeroInit )
    {
        return GraphicsDeviceBuffer( count * sizeof( Type ), access, zeroInit );
    }

    template<class Type>
    Type * GraphicsDeviceBuffer::get( std::size_t byteOffset )
    {
        return reinterpret_cast<Type *>( get( byteOffset ) );
    }

    template<class Type>
    const Type * GraphicsDeviceBuffer::get( std::size_t byteOffset ) const
    {
        return reinterpret_cast<const Type *>( get( byteOffset ) );
    }

    template<class Type>
    GraphicsDeviceBuffer::ScopedMapping<Type> GraphicsDeviceBuffer::scopedMap( std::size_t byteOffset )
    {
        Type * ptr = get<Type>( byteOffset );
        return ScopedMapping<Type>( ptr, [ this ]( Type * ) { unmap(); } );
    }

    template<class Type>
    std::size_t GraphicsDeviceBuffer::size() const
    {
        return m_size / sizeof( Type );
    }

    template<class Type>
    std::vector<Type> GraphicsDeviceBuffer::toHost()
    {
        assert( m_size % sizeof( Type ) == 0 && "It seems that this type is not suitable." );

        const std::size_t hostBufferSize = m_size / sizeof( Type );
        std::vector<Type> buffer         = std::vector<Type>( hostBufferSize );

        mmemcpy<MemcpyType::DeviceToHost>( reinterpret_cast<uint8_t *>( buffer.data() ), m_ptr, m_size );
        return buffer;
    }

    inline uint8_t *       DeviceBuffer::get() { return m_ptr; }
    inline const uint8_t * DeviceBuffer::get() const { return m_ptr; }

    template<class Type>
    Type * DeviceBuffer::get( std::size_t offset )
    {
        return reinterpret_cast<Type *>( m_ptr + offset );
    }

    template<class Type>
    const Type * DeviceBuffer::get( std::size_t offset ) const
    {
        return reinterpret_cast<const Type *>( m_ptr + offset );
    }

    template<class Type>
    std::size_t DeviceBuffer::size() const
    {
        return m_size / sizeof( Type );
    }

    template<class Type>
    std::vector<Type> DeviceBuffer::toHost()
    {
        assert( m_size % sizeof( Type ) == 0 && "It seems that this type is not suitable." );

        const std::size_t hostBufferSize = m_size / sizeof( Type );
        std::vector<Type> buffer         = std::vector<Type>( hostBufferSize );

        mmemcpy<MemcpyType::DeviceToHost>( reinterpret_cast<uint8_t *>( buffer.data() ), m_ptr, m_size );
        return buffer;
    }

    inline uint8_t * ResultBuffer::get()
    {
        if(m_isGraphics)
            return m_graphicsBuffer.get();

        return m_buffer.get();
    }

    inline const uint8_t * ResultBuffer::get() const
    {
        if(m_isGraphics)
            return m_graphicsBuffer.get();

        return m_buffer.get();
    }

    template<class Type>
    Type * ResultBuffer::get( std::size_t offset )
    {
        if(m_isGraphics)
            return m_graphicsBuffer.get<Type>(offset);

        return m_buffer.get<Type>(offset);
    }

    template<class Type>
    const Type * ResultBuffer::get( std::size_t offset  ) const
    {
        if(m_isGraphics)
            return m_graphicsBuffer.get<Type>(offset);

        return m_buffer.get<Type>(offset);
    }

    template<class Type>
    std::size_t ResultBuffer::size() const
    {
        if(m_isGraphics)
            return m_graphicsBuffer.size<Type>();

        return m_buffer.size<Type>();
    }

    template<class Type>
    std::vector<Type> ResultBuffer::toHost()
    {
        if(m_isGraphics)
            return m_graphicsBuffer.toHost<Type>();

        return m_buffer.toHost<Type>();
    }

} // namespace bcs