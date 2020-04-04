
#include "device_math_vec.hpp"
#include "optix_parameters.hpp"
#include <optix_device.h>

namespace VTX
{
	namespace Renderer
	{
		extern "C" __constant__ Optix::LaunchParameters params;

		static __forceinline__ __device__ void setPayload( const float3 & p )
		{
			optixSetPayload_0( float_as_int( p.x ) );
			optixSetPayload_1( float_as_int( p.y ) );
			optixSetPayload_2( float_as_int( p.z ) );
		}

		static __forceinline__ __device__ float3 getPayload()
		{
			return make_float3( int_as_float( optixGetPayload_0() ),
								int_as_float( optixGetPayload_1() ),
								int_as_float( optixGetPayload_2() ) );
		}

		__forceinline__ __device__ uchar4 make_color( const float3 & c )
		{
			return make_uchar4( static_cast<uint8_t>( clamp( c.x, 0.f, 1.f ) * 255.f ),
								static_cast<uint8_t>( clamp( c.y, 0.f, 1.f ) * 255.f ),
								static_cast<uint8_t>( clamp( c.z, 0.f, 1.f ) * 255.f ),
								255u );
		}

		extern "C" __global__ void __closesthit__sphere()
		{
			Optix::HitGroupData * data = reinterpret_cast<Optix::HitGroupData *>( optixGetSbtDataPointer() );

			const float3 normal = make_float3( int_as_float( optixGetAttribute_0() ),
													  int_as_float( optixGetAttribute_1() ),
													  int_as_float( optixGetAttribute_2() ) );
			const int	 id		= optixGetPrimitiveIndex();
			const float3 & color	= params._colors[data->_spheres[ id ]._colorId];
			const float3 &rayDir = optixGetWorldRayDirection();
			const float	 radiance = fabsf( dot( rayDir, normal ) );


			setPayload( color * radiance);
		}

		extern "C" __global__ void __closesthit__cylinder()
		{
			Optix::HitGroupData * data = reinterpret_cast<Optix::HitGroupData *>( optixGetSbtDataPointer() );

			const float3   normal = make_float3( int_as_float( optixGetAttribute_0() ),
												 int_as_float( optixGetAttribute_1() ),
												 int_as_float( optixGetAttribute_2() ) );
			const int	   id	  = optixGetPrimitiveIndex();
			const float3 & color	= params._colors[data->_cylinders[ id ]._colorId];
			const float3 & rayDir	= optixGetWorldRayDirection();
			const float	   radiance = fabsf( dot( rayDir, normal ) );

			setPayload( color * radiance );
		}

		//extern "C" __global__ void __anyhit__() {}

		extern "C" __global__ void __miss__()
		{
			Optix::MissData * data	  = reinterpret_cast<Optix::MissData *>( optixGetSbtDataPointer() );
			float3			  payload = getPayload();
			setPayload( data->_colorBackground );
		}

		static __forceinline__ __device__ void trace( const OptixTraversableHandle & th,
													  const float3					 &rayOrigin,
													  const float3					 &rayDirection,
													  const float					 tMin,
													  const float					 tMax,
													  float3 *						 perRayData )
		{
			uint32_t p0, p1, p2;
			p0 = float_as_int( perRayData->x );
			p1 = float_as_int( perRayData->y );
			p2 = float_as_int( perRayData->z );

			optixTrace( th, // GAS
						rayOrigin,
						rayDirection,
						tMin,
						tMax,
						0.f, // ray time
						OptixVisibilityMask( 1 ),
						OPTIX_RAY_FLAG_NONE,
						0, // SBT offset
						0, // SBT stride
						0, // miss SBT index
						p0,
						p1,
						p2 );

			perRayData->x = int_as_float( p0 );
			perRayData->y = int_as_float( p1 );
			perRayData->z = int_as_float( p2 );
		}

		extern "C" __global__ void __raygen__()
		{
			const uint3 &id	= optixGetLaunchIndex();
			const uint3 &dim = optixGetLaunchDimensions();

			const Optix::RayGeneratorData * data
				= reinterpret_cast<Optix::RayGeneratorData *>( optixGetSbtDataPointer() );

			// camera data
			const float3 & origin = data->_camera._position;
			const float3 & front  = data->_camera._front;
			const float3 & du	  = data->_camera._du;
			const float3 & dv	  = data->_camera._dv;

			const float2 d = make_float2( float( id.x ) / float( dim.x ), float( id.y ) / float( dim.y ) ) * 2.f - 1.f;

			const float3 rayDir = normalize( du * d.x + dv * d.y + front );

			const uint32_t frameBufferId = id.x + id.y * dim.x;

			float3 normal;

			trace( params._traversable, // GAS
				   origin,
				   rayDir,
				   1e-3f, // tMin
				   1e32f, // tMax
				   &normal );

			params._frame._pixels[ frameBufferId ] = make_color( normal );
		}

		extern "C" __global__ void __intersection__sphere()
		{
			Optix::HitGroupData * data = reinterpret_cast<Optix::HitGroupData *>( optixGetSbtDataPointer() );

			// primitive data
			const int	   id	  = optixGetPrimitiveIndex();
			
			Optix::Intersection hit;
			if (data->_spheres[id].intersect(
				optixGetObjectRayOrigin(), optixGetObjectRayDirection(), optixGetRayTmin(), optixGetRayTmax(), hit))
			{
				unsigned int p0 = float_as_int( hit._normal.x );
				unsigned int p1 = float_as_int( hit._normal.y );
				unsigned int p2 = float_as_int( hit._normal.z );

				optixReportIntersection( hit._t, 0, p0, p1, p2 );
			}
		}

		extern "C" __global__ void __intersection__cylinder()
		{
			Optix::HitGroupData * data = reinterpret_cast<Optix::HitGroupData *>( optixGetSbtDataPointer() );

			// primitive data
			const int id = optixGetPrimitiveIndex();

			Optix::Intersection hit;
			if ( data->_cylinders[ id ].intersect( optixGetObjectRayOrigin(),
												 optixGetObjectRayDirection(),
												 optixGetRayTmin(),
												 optixGetRayTmax(),
												 hit ) )
			{
				unsigned int p0 = float_as_int( hit._normal.x );
				unsigned int p1 = float_as_int( hit._normal.y );
				unsigned int p2 = float_as_int( hit._normal.z );

				optixReportIntersection( hit._t, 0, p0, p1, p2 );
			}
		}
	} // namespace Renderer
} // namespace VTX
