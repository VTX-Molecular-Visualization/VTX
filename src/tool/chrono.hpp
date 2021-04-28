#ifndef __VTX_CHRONO__
#define __VTX_CHRONO__

#ifdef _MSC_VER
#pragma once
#endif

#include <chrono>
#include <string>

namespace VTX
{
	namespace Tool
	{
		class Chrono
		{
			using Clock	   = std::chrono::high_resolution_clock;
			using Duration = std::chrono::duration<float>;

		  public:
			void  start() { begin = interval = Clock::now(); }
			void  stop() { end = interval = Clock::now(); }
			float elapsedTime() const { return ( std::chrono::duration_cast<Duration>( end - begin ) ).count(); }
			float intervalTime()
			{
				Duration intervalTime = std::chrono::duration_cast<Duration>( Clock::now() - interval );
				interval			  = Clock::now();
				return ( intervalTime ).count();
			}

		  private:
			Clock::time_point begin;
			Clock::time_point interval;
			Clock::time_point end;
		};
	} // namespace Tool
} // namespace VTX
#endif
