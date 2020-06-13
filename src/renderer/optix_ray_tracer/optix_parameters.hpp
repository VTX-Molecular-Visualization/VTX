#ifndef __VTX_RENDERER_OPTIX_LAUNCH_PARAMETERS__
#define __VTX_RENDERER_OPTIX_LAUNCH_PARAMETERS__

#ifdef _MSC_VER
#pragma once
#endif

//#ifndef OPTIX_DEFINED

#include "cylinder.hpp"
#include "sphere.hpp"

namespace VTX::Renderer::Optix
{
	enum
	{
		RADIANCE_RAY_TYPE = 0,
		SHADOW_RAY_TYPE,
		RAY_TYPE_COUNT
	};

	struct LaunchParameters
	{
		struct
		{
			uchar4 * _pixels = nullptr;
			uint16_t _width	 = 0;
			uint16_t _height = 0;
			uint32_t _id	 = 0;
		} _frame;

		float3 * _colors;

		OptixTraversableHandle _traversable = 0ull;
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

		int _nbSamples;
	};

	struct MissData
	{
		float3 _colorBackground;
	};

	struct HitGroupData
	{
		Sphere *   _spheres;
		Cylinder * _cylinders;
	};

	template<typename T>
	struct __align__( OPTIX_SBT_RECORD_ALIGNMENT ) Record
	{
		__align__( OPTIX_SBT_RECORD_ALIGNMENT ) char header[ OPTIX_SBT_RECORD_HEADER_SIZE ];

		T _data;
	};

	using RayGeneratorRecord = Record<RayGeneratorData>;
	using MissRecord		 = Record<MissData>;
	using HitGroupRecord	 = Record<HitGroupData>;

} // namespace VTX::Renderer::Optix

//#endif
#endif
