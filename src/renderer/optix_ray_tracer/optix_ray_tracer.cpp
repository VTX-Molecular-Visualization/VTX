#include "optix_ray_tracer.hpp"
#include "optix_util.hpp"
#include "tool/chrono.hpp"
#include "util/sampler.hpp"
#include "vtx_app.hpp"
#include <atomic>
#include <stb/stb_image_write.h>
#include <thread>

namespace VTX
{
	namespace Renderer
	{
		int chooseBestDevice()
		{
			// Get number of CUDA capable devices
			int nbDevices;
			cudaGetDeviceCount( &nbDevices );

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

		void OptixRayTracer::init( const uint p_width, const uint p_height )
		{
			VTX_INFO( "Initializing OptiX ray tracer (only first molecule)..." );

			BaseRenderer::resize( p_width, p_height );

			// check for CUDA capable devices
			cudaFree( 0 );

			int bestDevice = chooseBestDevice();

			VTX_INFO( "CUDA device capable found" );
			cudaDeviceProp deviceProperties;
			cudaGetDeviceProperties( &deviceProperties, bestDevice );

			std::cout << deviceProperties.name << std::endl;
			std::cout << "Compute capability: " << deviceProperties.major << "." << deviceProperties.minor << std::endl;

			if ( false /*TODO: check what compute capability we need*/ )
			{ throw std::runtime_error( "no CUDA capable device is detected with compute capability >= X.Y" ); }

			OPTIX_HANDLE_ERROR( optixInit() );

			VTX_INFO( "Optix ray tracer initialized" );
		}

		void OptixRayTracer::renderFrame( const Object3D::Scene & p_scene )
		{
			VTX_INFO( "Render Frame" );

			// start rendering
			Tool::Chrono chrono;

			chrono.start();

			chrono.stop();

			const double time = chrono.elapsedTime();

			VTX_INFO( "Rendering time: " + std::to_string( time ) );
			VTX_INFO( "Save image as: test RT.png" );

			// stbi_write_png( "test RT.png", _width, _height, 3, pixels.data(), 0 );
		}

		void OptixRayTracer::setShading() {}

		void OptixRayTracer::resize( const uint, const uint ) { BaseRenderer::resize( _width, _height ); }

	} // namespace Renderer
} // namespace VTX
