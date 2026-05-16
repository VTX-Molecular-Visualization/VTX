#include "renderer/context/backend/interop/opengl_cuda_interop.hpp"
#include "renderer/context/backend/gl/include_opengl.hpp"
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
		void _cudaCheck( const cudaError_t p_error, const char * const p_context )
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

	void OpenGLCudaInterop::refreshAvailability()
	{
#ifdef VTX_CUDA_ENABLED
		_impl->availability = { true, false };

		int deviceCount = 0;
		if ( cudaGetDeviceCount( &deviceCount ) != cudaSuccess || deviceCount == 0 )
		{
			return;
		}
		_impl->availability.runtime = true;

		uint cudaGlDeviceCount = 0;
		int	 cudaGlDevices[ 16 ] {};
		if ( cudaGLGetDevices( &cudaGlDeviceCount, cudaGlDevices, 16, cudaGLDeviceListAll ) != cudaSuccess )
		{
			return;
		}

		// No device with CUDA-OpenGL interop found.
		_impl->availability.runtime = cudaGlDeviceCount > 0;
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

	Desc::InteropBufferMapping OpenGLCudaInterop::mapBuffer(
		const Desc::Key &		   p_key,
		const uint				   p_graphicsBufferId,
		const Desc::E_BUFFER_USAGE p_usage
	)
	{
#ifdef VTX_CUDA_ENABLED
		if ( not _impl->availability.available() )
		{
			throw GraphicException( "CUDA graphics interop is not available" );
		}

		if ( not Util::Enum::hasBits( p_usage, Desc::E_BUFFER_USAGE::CUDA_READ )
			 && not Util::Enum::hasBits( p_usage, Desc::E_BUFFER_USAGE::CUDA_WRITE ) )
		{
			throw GraphicException( "Buffer '{}' is not declared with CUDA usage", p_key );
		}

		Impl::Registration & registration = _impl->registrations[ p_key ];
		if ( registration.resource == nullptr )
		{
			uint flags = cudaGraphicsRegisterFlagsNone;
			if ( Util::Enum::hasBits( p_usage, Desc::E_BUFFER_USAGE::CUDA_READ )
				 && not Util::Enum::hasBits( p_usage, Desc::E_BUFFER_USAGE::CUDA_WRITE ) )
			{
				flags = cudaGraphicsRegisterFlagsReadOnly;
			}
			else if ( not Util::Enum::hasBits( p_usage, Desc::E_BUFFER_USAGE::CUDA_READ )
					  && Util::Enum::hasBits( p_usage, Desc::E_BUFFER_USAGE::CUDA_WRITE ) )
			{
				flags = cudaGraphicsRegisterFlagsWriteDiscard;
			}

			_cudaCheck(
				cudaGraphicsGLRegisterBuffer( &registration.resource, p_graphicsBufferId, flags ),
				"Could not register graphics buffer for CUDA interop"
			);
		}

		if ( not registration.mapped )
		{
			_cudaCheck( cudaGraphicsMapResources( 1, &registration.resource ), "Could not map CUDA interop buffer" );
			_cudaCheck(
				cudaGraphicsResourceGetMappedPointer(
					&registration.devicePtr, &registration.size, registration.resource
				),
				"Could not get CUDA device pointer from graphics buffer"
			);
			registration.mapped = true;
		}

		return { p_key, registration.devicePtr, registration.size };
#else
		return {};
#endif
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
