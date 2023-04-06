#ifndef __VTX_STAT__
#define __VTX_STAT__

namespace VTX
{
	struct Stat
	{
		uint  tickRate	 = 0;
		uint  FPS		 = 666u;
		float renderTime = 0.f; // Miliseconds.
		uint  drawCalls	 = 0u;
	};
} // namespace VTX
#endif
