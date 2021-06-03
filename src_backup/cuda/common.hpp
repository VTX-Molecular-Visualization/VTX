#ifndef __VTX_UTIL_CUDA_COMMON__
#define __VTX_UTIL_CUDA_COMMON__

#ifdef _MSC_VER
#pragma once
#endif

#include "exception.hpp"
#include <cuda.h>
#include <cuda_runtime.h>
#include <nvrtc.h>
#include <sstream>
#include <stdexcept>

#ifdef __CUDACC__
#define VTX_HOST_DEVICE __host__ __device__
#define VTX_INLINE_DEVICE __forceinline__ __device__
#define VTX_INLINE_HOST_DEVICE __forceinline__ __host__ __device__
#else
#define VTX_HOST_DEVICE
#define VTX_INLINE_DEVICE inline
#define VTX_INLINE_HOST_DEVICE inline
#endif

// TODO: remove marco !
#define CUDA_HANDLE_ERROR( p_res )                                                                          \
	do                                                                                                      \
	{                                                                                                       \
		const cudaError_t res = p_res;                                                                      \
		if ( res != cudaSuccess )                                                                           \
		{                                                                                                   \
			std::stringstream message;                                                                      \
			message << "Error " << cudaGetErrorName( p_res ) << " (" << cudaGetErrorString( p_res ) << ")"; \
			throw Exception::CudaException( message.str() );                                                \
		}                                                                                                   \
	} while ( 0 )

#define CUDA_SYNCHRONIZE_HANDLE_ERROR()                                   \
	do                                                                    \
	{                                                                     \
		cudaDeviceSynchronize();                                          \
		const cudaError_t res = cudaGetLastError();                       \
		if ( res != cudaSuccess )                                         \
		{                                                                 \
			std::stringstream message;                                    \
			message << "Synchronize error " << cudaGetErrorString( res ); \
			throw Exception::CudaException( message.str() );              \
		}                                                                 \
	} while ( 0 )

// TODO: remove marco !
// TODO: and remove from here !
#define NVRTC_HANDLE_ERROR( p_res )                                                             \
	do                                                                                          \
	{                                                                                           \
		nvrtcResult res = p_res;                                                                \
		if ( res != NVRTC_SUCCESS )                                                             \
		{                                                                                       \
			std::stringstream message;                                                          \
			message << "NVRTC Error " << #p_res << " (" << nvrtcGetErrorString( p_res ) << ")"; \
			throw std::runtime_error( message.str() );                                          \
		}                                                                                       \
	} while ( 0 )

namespace VTX::CUDA
{
	static const int chooseBestDevice()
	{
		// Get number of CUDA capable devices
		int nbDevices;
		CUDA_HANDLE_ERROR( cudaGetDeviceCount( &nbDevices ) );

		if ( nbDevices == 0 )
		{
			throw std::runtime_error( "Cannot find CUDA capable device" );
		}

		// Choose best device
		int			   currentDev = 0;
		int			   bestDev	  = -1;
		int			   bestMajor  = 0;
		cudaDeviceProp propDev;

		while ( currentDev < nbDevices )
		{
			cudaGetDeviceProperties( &propDev, currentDev );
			if ( propDev.major > bestMajor )
			{
				bestDev	  = currentDev;
				bestMajor = propDev.major;
			}
			++currentDev;
		}
		return bestDev;
	}
} // namespace VTX::CUDA
#endif
