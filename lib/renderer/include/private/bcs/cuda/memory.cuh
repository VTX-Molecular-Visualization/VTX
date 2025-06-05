#ifndef BCS_CUDA_MEMORY_CUH
#define BCS_CUDA_MEMORY_CUH

#include <device_types.h>
#include <thrust/async/copy.h>
#include <thrust/execution_policy.h>

#include "bcs/core/meta.hpp"
#include "bcs/cuda/setup.cuh"

#define GL_INVALID_VALUE 0x0501
using GLuint = unsigned int;

namespace bcs
{
    enum class MemcpyType
    {
        DeviceToHost,
        HostToDevice
    };

    template<MemcpyType type, class Type>
    void mmemcpy( Type * const dst, const Type * const src, std::size_t count )
    {
        const std::size_t size = count * sizeof( Type );
        if constexpr ( type == MemcpyType::HostToDevice )
        {
            cudaCheck( cudaMemcpy( dst, src, size, cudaMemcpyHostToDevice ) );
        }
        else if constexpr ( type == MemcpyType::DeviceToHost )
        {
            cudaCheck( cudaMemcpy( dst, src, size, cudaMemcpyDeviceToHost ) );
        }
    }

    template<class Type>
    void copy( Type * const __restrict__ dst, const Type * const __restrict__ src, uint32_t count );

    template<class Type1, class Type2>
    __host__ void prefixSumCount( Type1 * src, Type2 * dst, Type2 * countValue, uint32_t size );

    template<class Type>
    void loadDeviceMemory( Type * hostDst, Type * deviceSrc, std::size_t count = 1 )
    {
        mmemcpy<MemcpyType::DeviceToHost>( hostDst, deviceSrc, count );
    }

    enum class AccessType : uint8_t
    {
        None      = 0,
        Read      = 1,
        Write     = 2,
        ReadWrite = 3
    };
    BCS_DEFINE_ENUM_BITWISE_FUNCTION( AccessType )

    class GraphicsDeviceBuffer
    {
      public:
        template<class Type>
        static GraphicsDeviceBuffer Typed( std::size_t count,
                                           AccessType  access   = AccessType::ReadWrite,
                                           bool        zeroInit = false );

        GraphicsDeviceBuffer() = default;
        GraphicsDeviceBuffer( const std::size_t size,
                              AccessType        access   = AccessType::ReadWrite,
                              bool              zeroInit = false );

        GraphicsDeviceBuffer( GraphicsDeviceBuffer & other ) = delete;
        GraphicsDeviceBuffer & operator=( const GraphicsDeviceBuffer & other ) = delete;

        GraphicsDeviceBuffer( GraphicsDeviceBuffer && other ) noexcept;
        GraphicsDeviceBuffer & operator=( GraphicsDeviceBuffer && other ) noexcept;

        ~GraphicsDeviceBuffer();

        template<class Type>
        Type * get( std::size_t byteOffset = 0 );
        template<class Type>
        const Type * get( std::size_t byteOffset = 0 ) const;

        uint8_t * get( std::size_t byteOffset = 0 );
        uint8_t * get( std::size_t byteOffset = 0 ) const;

        void unmap() const;

        template<class Type>
        using ScopedMapping = std::unique_ptr<Type, std::function<void( Type * )>>;

        template<class Type>
        ScopedMapping<Type> scopedMap( std::size_t byteOffset = 0 );

        GLuint getId() const;

        std::size_t size() const;

        template<class Type>
        std::size_t size() const;

        template<class Type>
        std::vector<Type> toHost();

        operator bool() const;
      private:
        bool m_isInitialized = false;

        std::size_t       m_size       = 0;
        mutable uint8_t * m_ptr        = nullptr;
        AccessType        m_accessType = AccessType::ReadWrite;

        // GL interoperability
        GLuint                         m_bufferId = GL_INVALID_VALUE;
        uint32_t                       m_type     = GL_INVALID_VALUE;
        mutable cudaGraphicsResource_t m_binding  = nullptr;
        cudaGraphicsRegisterFlags      m_registerFlags{};
    };

    class DeviceBuffer
    {
      public:
        template<class Type>
        static DeviceBuffer Typed( const std::size_t count, bool zeroInit = false )
        {
            return { count * sizeof( Type ), zeroInit };
        }

        DeviceBuffer() = default;
        DeviceBuffer( const std::size_t size, bool zeroInit = false );

        DeviceBuffer( const DeviceBuffer & ) = delete;
        DeviceBuffer & operator=( const DeviceBuffer & ) = delete;

        DeviceBuffer( DeviceBuffer && other ) noexcept;
        DeviceBuffer & operator=( DeviceBuffer && other ) noexcept;
        ~DeviceBuffer();

        void reset();

        inline uint8_t *       get();
        inline const uint8_t * get() const;

        template<class Type>
        Type * get( std::size_t offset = 0 );
        template<class Type>
        const Type * get( std::size_t offset = 0 ) const;

        operator bool() const;

        template<class Type>
        std::size_t size() const;
        std::size_t size() const;

        template<class Type>
        std::vector<Type> toHost();

      private:
        bool        m_initialized = false;
        std::size_t m_size        = 0;
        uint8_t *   m_ptr         = nullptr;
    };

    class ResultBuffer
    {
      public:
        template<class Type>
        static ResultBuffer Typed( const std::size_t count, bool zeroInit = false, bool graphics = false )
        {
            return { count * sizeof( Type ), zeroInit, graphics };
        }

        ResultBuffer() = default;
        ResultBuffer( const std::size_t size, bool zeroInit = false, bool graphics = false );

        ResultBuffer( const ResultBuffer & ) = delete;
        ResultBuffer & operator=( const ResultBuffer & ) = delete;

        ResultBuffer( ResultBuffer && other ) noexcept;
        ResultBuffer & operator=( ResultBuffer && other ) noexcept;

        ~ResultBuffer();

        void unmap() const;

        inline uint8_t *       get();
        inline const uint8_t * get() const;

        template<class Type>
        Type * get( std::size_t offset = 0 );
        template<class Type>
        const Type * get( std::size_t offset = 0 ) const;

        operator bool() const;

        template<class Type>
        std::size_t size() const;
        std::size_t size() const;

        template<class Type>
        std::vector<Type> toHost();

        GLuint getId() const;
      private:
        bool m_isGraphics = false;

        GraphicsDeviceBuffer m_graphicsBuffer{};
        DeviceBuffer m_buffer{};
    };

} // namespace bcs

#include "bcs/cuda/memory.inl"

#endif // BCS_CUDAMEMORY_CUH
