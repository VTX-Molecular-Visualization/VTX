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
			void		start() { begin = Clock::now(); }
			void		stop() { end = Clock::now(); }
			float		elapsedTime() const { return ( std::chrono::duration_cast<Duration>( end - begin ) ).count(); }
			std::string elapsedTimeStr() const { return std::to_string( elapsedTime() ); }

		  private:
			Clock::time_point begin;
			Clock::time_point end;
		};
	} // namespace Tool
} // namespace VTX
#endif
