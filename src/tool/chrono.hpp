#ifndef __VTX_CHRONO__
#define __VTX_CHRONO__

#include <chrono>

namespace VTX
{
	namespace Tool
	{
		class Chrono
		{
			using Clock	   = std::chrono::high_resolution_clock;
			using Duration = std::chrono::duration<double>;

		  public:
			void   start() { begin = Clock::now(); }
			void   stop() { end = Clock::now(); }
			double elapsedTime() const { return ( std::chrono::duration_cast<Duration>( end - begin ) ).count(); }

		  private:
			Clock::time_point begin;
			Clock::time_point end;
		};
	} // namespace Tool
} // namespace VTX
#endif
