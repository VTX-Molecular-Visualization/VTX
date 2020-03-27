
#include "optix_launch_parameters.hpp"
#include <optix_device.h>

namespace VTX
{
	namespace Renderer
	{
		/*! launch parameters in constant memory, filled in by optix upon
			optixLaunch (this gets filled in from the buffer we pass to
			optixLaunch) */
		extern "C" __constant__ OptixLaunchParameters params;

		//------------------------------------------------------------------------------
		// closest hit and anyhit programs for radiance-type rays.
		//
		// Note eventually we will have to create one pair of those for each
		// ray type and each geometry type we want to render; but this
		// simple example doesn't use any actual geometries yet, so we only
		// create a single, dummy, set of them (we do have to have at leasts
		// one group of them to set up the SBT)
		//------------------------------------------------------------------------------

		extern "C" __global__ void __closesthit__radiance()
		{ /*! for this simple example, this will remain empty */
		}

		extern "C" __global__ void __anyhit__radiance()
		{ /*! for this simple example, this will remain empty */
		}

		//------------------------------------------------------------------------------
		// miss program that gets called for any ray that did not have a
		// valid intersection
		//
		// as with the anyhit/closest hit programs, in this example we only
		// need to have _some_ dummy function to set up a valid SBT
		// ------------------------------------------------------------------------------

		extern "C" __global__ void __miss__radiance()
		{ /*! for this simple example, this will remain empty */
		}

		//------------------------------------------------------------------------------
		// ray gen program - the actual rendering happens in here
		//------------------------------------------------------------------------------
		extern "C" __global__ void __raygen__renderFrame()
		{
			if ( params._frameId == 0 && optixGetLaunchIndex().x == 0 && optixGetLaunchIndex().y == 0 )
			{
				// we could of course also have used optixGetLaunchDims to query
				// the launch size, but accessing the params here
				// makes sure they're not getting optimized away (because
				// otherwise they'd not get used)
				printf( "############################################\n" );
				printf( "Hello world from OptiX 7 raygen program!\n(within a %ix%i-sized launch)\n",
						params._frameBufferDim.x,
						params._frameBufferDim.y );
				printf( "############################################\n" );
			}

			// ------------------------------------------------------------------
			// for this example, produce a simple test pattern:
			// ------------------------------------------------------------------

			// compute a test pattern based on pixel ID
			const int ix = optixGetLaunchIndex().x;
			const int iy = optixGetLaunchIndex().y;

			const int r = ( ix % 256 );
			const int g = ( iy % 256 );
			const int b = ( ( ix + iy ) % 256 );

			// convert to 32-bit rgba value (we explicitly set alpha to 0xff
			// to make stb_image_write happy ...
			const uint32_t rgba = 0xff000000 | ( r << 0 ) | ( g << 8 ) | ( b << 16 );

			// and write to frame buffer ...
			const uint32_t frameBufferId				   = ix + iy * params._frameBufferDim.x;
			params._colorBuffer[ frameBufferId ] = rgba;
		}
	} // namespace Renderer

} // namespace VTX
