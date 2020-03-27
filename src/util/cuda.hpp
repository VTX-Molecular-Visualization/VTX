#ifndef __VTX_CUDA__
#define __VTX_CUDA__

#ifdef _MSC_VER
#pragma once
#endif

#include <cuda.h>
#include <cuda_runtime.h>
#include <nvrtc.h>
#include <sstream>
#include <stdexcept>

// TODO: remove marco !
#define CUDA_HANDLE_ERROR( p_res )                                                                               \
	do                                                                                                           \
	{                                                                                                            \
		const cudaError_t res = p_res;                                                                           \
		if ( res != cudaSuccess )                                                                                \
		{                                                                                                        \
			std::stringstream message;                                                                           \
			message << "CUDA Error " << cudaGetErrorName( p_res ) << " (" << cudaGetErrorString( p_res ) << ")"; \
			throw std::runtime_error( message.str() );                                                           \
		}                                                                                                        \
	} while ( 0 )

#define CUDA_SYNCHRONIZE_HANDLE_ERROR()                                      \
	do                                                                       \
	{                                                                        \
		cudaDeviceSynchronize();                                             \
		const cudaError_t res = cudaGetLastError();                          \
		if ( res != cudaSuccess )                                            \
		{                                                                    \
			std::stringstream message;                                       \
			message << "CUDA Synchronize error " << cudaGetErrorName( res ); \
			throw std::runtime_error( message.str() );                       \
		}                                                                    \
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

namespace VTX
{
	namespace Util
	{
		namespace CUDA
		{
			static const int chooseBestDevice()
			{
				// Get number of CUDA capable devices
				int nbDevices;
				CUDA_HANDLE_ERROR( cudaGetDeviceCount( &nbDevices ) );

				if ( nbDevices == 0 ) { throw std::runtime_error( "Cannot find CUDA capable device" ); }

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
		} // namespace CUDA
	}	  // namespace Util
} // namespace VTX

#endif
