#ifndef __VTX_RENDERER_RAY_TRACER_SCENE__
#define __VTX_RENDERER_RAY_TRACER_SCENE__

#ifdef _MSC_VER
#pragma once
#endif

#include "bvh.hpp"
#include "define.hpp"

namespace VTX
{
	namespace Renderer
	{
		struct Scene
		{
			BVH _bvh;
		};
	} // namespace Renderer
} // namespace VTX

#endif
