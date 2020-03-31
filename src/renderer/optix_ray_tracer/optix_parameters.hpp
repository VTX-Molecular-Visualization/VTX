#ifndef __VTX_RENDERER_OPTIX_LAUNCH_PARAMETERS__
#define __VTX_RENDERER_OPTIX_LAUNCH_PARAMETERS__

#ifdef _MSC_VER
#pragma once
#endif

#include <cuda_runtime.h>
#include <optix.h>
#include <stdint.h>

namespace VTX
{
	namespace Renderer
	{
		namespace Optix
		{
			struct LaunchParameters
			{
				struct
				{
					uchar4 * _pixels = nullptr;
					uint32_t _width;
					uint32_t _height;
					uint32_t _id;
				} _frame;

				OptixTraversableHandle _traversable;
			};

			struct RayGeneratorData
			{
				struct
				{
					float3 _position;
					float3 _front;
					float3 _du;
					float3 _dv;
				} _camera;
			};

			struct MissData
			{
				float3 _colorBackground;
			};

			struct HitGroupData
			{
				float3 * _positions;
				float *	 _radii;
				float3 * _colors;
			};

			template<typename T>
			struct __align__( OPTIX_SBT_RECORD_ALIGNMENT ) Record
			{
				__align__( OPTIX_SBT_RECORD_ALIGNMENT ) char header[ OPTIX_SBT_RECORD_HEADER_SIZE ];

				T _data;
			};

			using RayGeneratorRecord = Record<Optix::RayGeneratorData>;
			using MissRecord		 = Record<Optix::MissData>;
			using HitGroupRecord	 = Record<Optix::HitGroupData>;
		} // namespace Optix
	}	  // namespace Renderer
} // namespace VTX

#endif
