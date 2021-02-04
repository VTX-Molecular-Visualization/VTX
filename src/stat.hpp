#ifndef __VTX_STAT__
#define __VTX_STAT__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	struct Stat
	{
		uint  FPS		 = 666u;
		float renderTime = 0.f; // Miliseconds.
	};
} // namespace VTX
#endif
