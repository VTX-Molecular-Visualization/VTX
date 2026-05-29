#include "renderer/context/backend/interop/opengl_cuda_interop.hpp"
#include "renderer/context/backend/gl/include_opengl.hpp"
#include <array>
#include <string>
#include <unordered_map>
#include <util/enum.hpp>
#include <util/exceptions.hpp>

#ifdef VTX_CUDA_ENABLED
#include <cuda_gl_interop.h>
#include <cuda_runtime_api.h>
#endif

namespace VTX::Renderer::Context::Backend::Interop
{
#ifdef VTX_CUDA_ENABLED
	namespace
	{
		void _cudaCheck( const cudaError_t p_error, const std::string & p_context )
		{
			if ( p_error != cudaSuccess )
			{
				throw GraphicException( "{}: {}", p_context, cudaGetErrorString( p_error ) );
			}
		}
	} // namespace

	/**
	 * @brief Implementation struct.
	 */
	struct OpenGLCudaInterop::Impl
	{
		struct Registration
		{
			cudaGraphicsResource_t resource	 = nullptr;
			void *				   devicePtr = nullptr;
			size_t				   size		 = 0;
			bool				   mapped	 = false;
		};

		Desc::InteropAvailability					availability { true, false };
		std::unordered_map<Desc::Key, Registration> registrations;
	};
#else
	struct OpenGLCudaInterop::Impl
	{
		Desc::InteropAvailability availability {};
	};
#endif

	OpenGLCudaInterop::OpenGLCudaInterop() : _impl( std::make_unique<Impl>() ) {}

	OpenGLCudaInterop::~OpenGLCudaInterop() { clear(); }

	void OpenGLCudaInterop::refreshAvailability( const uint p_probeBuffer )
	{
#ifdef VTX_CUDA_ENABLED
		_impl->availability = { true, false };

		int deviceCount = 0;
		if ( cudaGetDeviceCount( &deviceCount ) != cudaSuccess || deviceCount == 0 )
		{
			return;
		}
		if ( p_probeBuffer == 0 )
		{
			return;
		}

		cudaGraphicsResource_t resource = nullptr;
		if ( cudaGraphicsGLRegisterBuffer( &resource, p_probeBuffer, cudaGraphicsRegisterFlagsNone ) != cudaSuccess )
		{
			return;
		}

		void * devicePtr = nullptr;
		size_t size		= 0;
		const bool mapped = cudaGraphicsMapResources( 1, &resource ) == cudaSuccess;
		if ( mapped && cudaGraphicsResourceGetMappedPointer( &devicePtr, &size, resource ) == cudaSuccess )
		{
			_impl->availability.runtime = devicePtr != nullptr && size > 0;
		}
		if ( mapped )
		{
			cudaGraphicsUnmapResources( 1, &resource );
		}

		cudaGraphicsUnregisterResource( resource );
#else
		_impl->availability = {};
#endif
	}

	Desc::InteropAvailability OpenGLCudaInterop::availability() const { return _impl->availability; }

	void OpenGLCudaInterop::clear()
	{
#ifdef VTX_CUDA_ENABLED
		for ( auto & [ key, registration ] : _impl->registrations )
		{
			if ( registration.mapped )
			{
				cudaGraphicsUnmapResources( 1, &registration.resource );
			}
			if ( registration.resource != nullptr )
			{
				cudaGraphicsUnregisterResource( registration.resource );
			}
		}

		_impl->registrations.clear();
#endif
	}

	std::vector<Desc::InteropBufferMapping> OpenGLCudaInterop::mapBuffers( std::span<const BufferRequest> p_requests )
	{
#ifdef VTX_CUDA_ENABLED
		if ( not _impl->availability.available() )
		{
			throw GraphicException( "CUDA graphics interop is not available" );
		}

		glFinish();

		for ( const BufferRequest & request : p_requests )
		{
			if ( not Util::Enum::hasAnyBit(
					 request.usage, Desc::E_BUFFER_USAGE::CUDA_READ | Desc::E_BUFFER_USAGE::CUDA_WRITE
				 ) )
			{
				throw GraphicException( "Buffer '{}' is not declared with CUDA usage", request.key );
			}

			Impl::Registration & registration = _impl->registrations[ request.key ];
			if ( registration.resource != nullptr )
			{
				continue;
			}

			uint flags = cudaGraphicsRegisterFlagsNone;
			if ( Util::Enum::hasAnyBit( request.usage, Desc::E_BUFFER_USAGE::CUDA_READ )
				 && not Util::Enum::hasAnyBit( request.usage, Desc::E_BUFFER_USAGE::CUDA_WRITE ) )
			{
				flags = cudaGraphicsRegisterFlagsReadOnly;
			}
			else if ( not Util::Enum::hasAnyBit( request.usage, Desc::E_BUFFER_USAGE::CUDA_READ )
					  && Util::Enum::hasAnyBit( request.usage, Desc::E_BUFFER_USAGE::CUDA_WRITE ) )
			{
				flags = cudaGraphicsRegisterFlagsWriteDiscard;
			}

			_cudaCheck(
				cudaGraphicsGLRegisterBuffer( &registration.resource, request.graphicsBufferId, flags ),
				"Could not register graphics buffer '" + request.key + "' for CUDA interop"
			);
		}

		std::vector<cudaGraphicsResource_t> resourcesToMap;
		resourcesToMap.reserve( p_requests.size() );
		for ( const BufferRequest & request : p_requests )
		{
			Impl::Registration & registration = _impl->registrations[ request.key ];
			if ( not registration.mapped )
			{
				resourcesToMap.emplace_back( registration.resource );
			}
		}

		if ( not resourcesToMap.empty() )
		{
			_cudaCheck(
				cudaGraphicsMapResources( int( resourcesToMap.size() ), resourcesToMap.data() ),
				"Could not map CUDA interop buffers"
			);
		}

		std::vector<Desc::InteropBufferMapping> result;
		result.reserve( p_requests.size() );

		for ( const BufferRequest & request : p_requests )
		{
			Impl::Registration & registration = _impl->registrations[ request.key ];
			_cudaCheck(
				cudaGraphicsResourceGetMappedPointer(
					&registration.devicePtr, &registration.size, registration.resource
				),
				"Could not get CUDA device pointer from graphics buffer"
			);
			registration.mapped = true;

			result.push_back( { request.key, registration.devicePtr, registration.size } );
		}

		return result;
#else
		return {};
#endif
	}

	Desc::InteropBufferMapping OpenGLCudaInterop::mapBuffer(
		const Desc::Key &		   p_key,
		const uint				   p_graphicsBufferId,
		const Desc::E_BUFFER_USAGE p_usage
	)
	{
		const std::array<BufferRequest, 1> requests {
			BufferRequest { p_key, p_graphicsBufferId, p_usage },
		};
		auto mappings = mapBuffers( requests );
		assert( mappings.size() == 1 );

		return std::move( mappings.front() );
	}

	void OpenGLCudaInterop::unmapBuffer( const Desc::Key & p_key )
	{
#ifdef VTX_CUDA_ENABLED
		const auto it = _impl->registrations.find( p_key );
		if ( it == _impl->registrations.end() || not it->second.mapped )
		{
			return;
		}

		_cudaCheck( cudaGraphicsUnmapResources( 1, &it->second.resource ), "Could not unmap CUDA interop buffer" );
		it->second.devicePtr = nullptr;
		it->second.size		 = 0;
		it->second.mapped	 = false;
#else
#endif
	}

	void OpenGLCudaInterop::unregisterBuffer( const Desc::Key & p_key )
	{
#ifdef VTX_CUDA_ENABLED
		const auto it = _impl->registrations.find( p_key );
		if ( it == _impl->registrations.end() )
		{
			return;
		}

		if ( it->second.mapped )
		{
			_cudaCheck( cudaGraphicsUnmapResources( 1, &it->second.resource ), "Could not unmap CUDA interop buffer" );
		}
		if ( it->second.resource != nullptr )
		{
			_cudaCheck(
				cudaGraphicsUnregisterResource( it->second.resource ),
				"Could not unregister graphics buffer from CUDA interop"
			);
		}

		_impl->registrations.erase( it );
#else
#endif
	}
} // namespace VTX::Renderer::Context::Backend::Interop
