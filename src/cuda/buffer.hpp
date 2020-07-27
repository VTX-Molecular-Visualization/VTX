#ifndef __VTX_CUDA_BUFFER__
#define __VTX_CUDA_BUFFER__

#ifdef _MSC_VER
#pragma once
#endif

#ifdef OPTIX_DEFINED

#include "cuda/common.hpp"
#include <assert.h>

namespace VTX::CUDA
{
	struct Buffer
	{
		inline CUdeviceptr getDevicePtr() const { return CUdeviceptr( _devicePtr ); }

		void malloc( const size_t p_size )
		{
			assert( _devicePtr == nullptr );
			_sizeInBytes = p_size;
			CUDA_HANDLE_ERROR( cudaMalloc( (void **)&_devicePtr, _sizeInBytes ) );
		}

		void free()
		{
			if ( _devicePtr != nullptr )
			{
				CUDA_HANDLE_ERROR( cudaFree( _devicePtr ) );
			}
			_devicePtr	 = nullptr;
			_sizeInBytes = 0;
		}

		template<typename T>
		void memcpyHostToDevice( const T * p_b, const size_t p_count )
		{
			assert( _devicePtr != nullptr );
			assert( _sizeInBytes == p_count * sizeof( T ) );
			CUDA_HANDLE_ERROR( cudaMemcpy( _devicePtr, (void *)p_b, p_count * sizeof( T ), cudaMemcpyHostToDevice ) );
		}

		template<typename T>
		void memcpyDeviceToHost( T * p_b, const size_t p_count )
		{
			assert( _devicePtr != nullptr );
			assert( _sizeInBytes == p_count * sizeof( T ) );
			CUDA_HANDLE_ERROR( cudaMemcpy( (void *)p_b, _devicePtr, p_count * sizeof( T ), cudaMemcpyDeviceToHost ) );
		}

		void realloc( const size_t p_size )
		{
			free();
			malloc( p_size );
		}

		void * _devicePtr	= nullptr;
		size_t _sizeInBytes = 0;
	};
} // namespace VTX::CUDA
#endif
#endif
