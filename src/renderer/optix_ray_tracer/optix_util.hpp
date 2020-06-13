#ifndef __VTX_OPTIX_UTIL__
#define __VTX_OPTIX_UTIL__

#ifdef _MSC_VER
#pragma once
#endif

#ifdef OPTIX_DEFINED

#include "define.hpp"
#include <optix.h>
#include <optix_stubs.h>

// TODO: remove macro
#define OPTIX_HANDLE_ERROR( p_res )                                                           \
	{                                                                                         \
		const OptixResult res = ( p_res );                                                    \
		if ( res != cudaSuccess )                                                             \
		{                                                                                     \
			std::stringstream message;                                                        \
			message << "Optix error (code " << res << "): " << #p_res << "line " << __LINE__; \
			throw Exception::OptixException( message.str() );                                 \
		}                                                                                     \
	}

#endif
#endif
