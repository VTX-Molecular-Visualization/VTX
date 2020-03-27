#ifndef __VTX_RENDERER_OPTIX_LAUNCH_PARAMETERS__
#define __VTX_RENDERER_OPTIX_LAUNCH_PARAMETERS__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"

namespace VTX
{
	namespace Renderer
	{
		struct OptixLaunchParameters
		{
			uint   _frameId = 0;
			uint * _colorBuffer;
			Vec2i  _frameBufferDim;
		};
	} // namespace Renderer
} // namespace VTX

#endif
