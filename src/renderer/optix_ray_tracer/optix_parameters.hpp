#ifndef __VTX_RENDERER_OPTIX_LAUNCH_PARAMETERS__
#define __VTX_RENDERER_OPTIX_LAUNCH_PARAMETERS__

#ifdef _MSC_VER
#pragma once
#endif

//#ifndef OPTIX_DEFINED

#include "cuda/vec.hpp"
#include "sphere.hpp"
#include <cuda_runtime.h>
#include <optix.h>
#include <stdint.h>

namespace VTX::Renderer::Optix
{
	enum
	{
		RADIANCE_RAY_TYPE = 0,
		SHADOW_RAY_TYPE,
		RAY_TYPE_COUNT
	};

	struct __align__( 32 ) LaunchParameters
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

	// TODO: useful align ?
	// struct __align__( 32 ) Cylinder
	//{
	//	Cylinder() = default;

	//
	//	uint16_t _padding;
	//};

	struct HitGroupData
	{
		Optix::Sphere * _spheres;
		// Cylinder *		_cylinders;
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

} // namespace VTX::Renderer::Optix

//#endif
#endif
