#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

#include <GL/gl3w.h>
#include <cuda_gl_interop.h>

#include "bcs/cuda/memory.cuh"

namespace bcs
{
    GraphicsDeviceBuffer::GraphicsDeviceBuffer( const std::size_t size, AccessType access, bool zeroInit ) :
        m_size( size ), m_registerFlags( cudaGraphicsRegisterFlagsNone ), m_isInitialized( true )
    {
        assert( size > 0 && "Trying to allocate GraphicsDeviceBuffer of size 0" );
        glCreateBuffers( 1, &m_bufferId );

        GLsizeiptr accessMask = 0;
        if ( ( access & AccessType::Read ) != AccessType::None )
            accessMask |= GL_MAP_READ_BIT;
        if ( ( access & AccessType::Write ) != AccessType::None )
            accessMask |= GL_MAP_WRITE_BIT;

        glNamedBufferStorage( m_bufferId, size, nullptr, accessMask );
        cudaCheck( cudaGraphicsGLRegisterBuffer( &m_binding, m_bufferId, m_registerFlags ) );

        if ( zeroInit )
        {
            const auto data = scopedMap<uint8_t>();
            cudaCheck( cudaMemset( data.get(), 0, size ) );
        }
    }

    GraphicsDeviceBuffer::GraphicsDeviceBuffer( GraphicsDeviceBuffer && other ) noexcept :
        m_registerFlags( cudaGraphicsRegisterFlagsNone )
    {
        std::swap( m_isInitialized, other.m_isInitialized );
        std::swap( m_size, other.m_size );
        std::swap( m_ptr, other.m_ptr );
        std::swap( m_accessType, other.m_accessType );
        std::swap( m_bufferId, other.m_bufferId );
        std::swap( m_type, other.m_type );
        std::swap( m_binding, other.m_binding );
        std::swap( m_registerFlags, other.m_registerFlags );
    }

    GraphicsDeviceBuffer & GraphicsDeviceBuffer::operator=( GraphicsDeviceBuffer && other ) noexcept
    {
        std::swap( m_isInitialized, other.m_isInitialized );
        std::swap( m_size, other.m_size );
        std::swap( m_ptr, other.m_ptr );
        std::swap( m_accessType, other.m_accessType );
        std::swap( m_bufferId, other.m_bufferId );
        std::swap( m_type, other.m_type );
        std::swap( m_binding, other.m_binding );
        std::swap( m_registerFlags, other.m_registerFlags );

        return *this;
    }

    GraphicsDeviceBuffer::~GraphicsDeviceBuffer()
    {
        if ( !m_isInitialized )
            return;

        unmap();
        cudaCheck( cudaGraphicsUnregisterResource( m_binding ) );
        glDeleteBuffers( 1, &m_bufferId );
    }

    uint8_t * GraphicsDeviceBuffer::get( std::size_t byteOffset )
    {
        if ( !m_ptr )
        {
            cudaCheck( cudaGraphicsMapResources( 1, &m_binding ) );
            cudaCheck(
                cudaGraphicsResourceGetMappedPointer( reinterpret_cast<void **>( &m_ptr ), nullptr, m_binding ) );
        }

        return m_ptr + byteOffset;
    }

    uint8_t * GraphicsDeviceBuffer::get( std::size_t byteOffset ) const
    {
        if ( !m_ptr )
        {
            cudaCheck( cudaGraphicsMapResources( 1, &m_binding ) );
            cudaCheck(
                cudaGraphicsResourceGetMappedPointer( reinterpret_cast<void **>( &m_ptr ), nullptr, m_binding ) );
        }

        return m_ptr + byteOffset;
    }

    void GraphicsDeviceBuffer::unmap() const
    {
        if ( !m_ptr )
            return;

        m_ptr = nullptr;
        cudaCheck( cudaGraphicsUnmapResources( 1, &m_binding ) );
    }
    GLuint GraphicsDeviceBuffer::getId() const { return m_bufferId; }

    std::size_t GraphicsDeviceBuffer::size() const { return m_size; }
    GraphicsDeviceBuffer::operator bool() const { return m_size > 0; }

    DeviceBuffer::DeviceBuffer( const std::size_t size, bool zeroInit ) : m_size( size )
    {
        cudaCheck( cudaMallocAsync( reinterpret_cast<void **>( &m_ptr ), size, 0 ) );
        if ( zeroInit )
            cudaCheck( cudaMemset( m_ptr, 0, size ) );

        m_initialized = true;
    }

    DeviceBuffer::DeviceBuffer( DeviceBuffer && other ) noexcept
    {
        std::swap( m_initialized, other.m_initialized );
        std::swap( m_size, other.m_size );
        std::swap( m_ptr, other.m_ptr );
    }

    DeviceBuffer & DeviceBuffer::operator=( DeviceBuffer && other ) noexcept
    {
        std::swap( m_initialized, other.m_initialized );
        std::swap( m_size, other.m_size );
        std::swap( m_ptr, other.m_ptr );

        return *this;
    }

    DeviceBuffer::~DeviceBuffer() { reset(); }

    void DeviceBuffer::reset()
    {
        if ( m_initialized )
        {
            cudaCheck( cudaFreeAsync( m_ptr, 0 ) );
            m_ptr         = nullptr;
            m_initialized = false;
        }
    }

    DeviceBuffer::operator bool() const { return m_initialized; }

    std::size_t DeviceBuffer::size() const { return m_size; }

    ResultBuffer::ResultBuffer( const std::size_t size, bool zeroInit, bool graphics ) : m_isGraphics( graphics )
    {
        if ( graphics )
            m_graphicsBuffer = GraphicsDeviceBuffer( size, AccessType::ReadWrite, zeroInit );
        else
            m_buffer = DeviceBuffer( size, zeroInit );
    }

    ResultBuffer::ResultBuffer( ResultBuffer && other ) noexcept
    {
        std::swap( m_isGraphics, other.m_isGraphics );
        std::swap( m_graphicsBuffer, other.m_graphicsBuffer );
        std::swap( m_buffer, other.m_buffer );
    }

    ResultBuffer & ResultBuffer::operator=( ResultBuffer && other ) noexcept
    {
        std::swap( m_isGraphics, other.m_isGraphics );
        std::swap( m_graphicsBuffer, other.m_graphicsBuffer );
        std::swap( m_buffer, other.m_buffer );

        return *this;
    }

    ResultBuffer::~ResultBuffer() = default;

    void ResultBuffer::unmap() const
    {
        if ( m_isGraphics )
            return m_graphicsBuffer.unmap();
    }

    ResultBuffer::operator bool() const
    {
        if ( m_isGraphics )
            return m_graphicsBuffer;

        return m_buffer;
    }

    std::size_t ResultBuffer::size() const
    {
        if ( m_isGraphics )
            return m_graphicsBuffer.size();

        return m_buffer.size();
    }

    GLuint ResultBuffer::getId() const { return m_graphicsBuffer.getId(); }
} // namespace bcs
