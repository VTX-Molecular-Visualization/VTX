#ifndef __VTX_OPTIX_UTIL__
#define __VTX_OPTIX_UTIL__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include <cuda_runtime.h>
#include <optix.h>
#include <optix_function_table_definition.h>
#include <optix_stubs.h>

#define OPTIX_HANDLE_ERROR( p_res )                                                             \
	{                                                                                           \
		const OptixResult res = ( p_res );                                                      \
		if ( res != cudaSuccess )                                                               \
		{                                                                                       \
			fprintf( stderr, "Optix error (code %d): %s  (line %d)\n", res, #p_res, __LINE__ ); \
			exit( EXIT_FAILURE );                                                               \
		}                                                                                       \
	}

#endif
