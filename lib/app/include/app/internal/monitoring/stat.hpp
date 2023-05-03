#ifndef __VTX_APP_INTERNAL_MONITORING_STAT__
#define __VTX_APP_INTERNAL_MONITORING_STAT__

namespace VTX::App::Internal::Monitoring
{
	struct Stat
	{
		unsigned int tickRate	= 0;
		unsigned int FPS		= 666u;
		float		 renderTime = 0.f; // Miliseconds.
		unsigned int drawCalls	= 0u;
	};
} // namespace VTX::App::Internal::Monitoring
#endif
