#ifndef __VTX_RENDERER_RAY_TRACER_OPTIX_INTERSECTION__
#define __VTX_RENDERER_RAY_TRACER_OPTIX_INTERSECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "cuda/vec.hpp"

namespace VTX::Renderer::Optix
{
	struct Intersection
	{
		float3 _point;
		float  _t;
		float3 _normal;
		int	   _colorId;
	};
} // namespace VTX::Renderer::Optix

#endif
