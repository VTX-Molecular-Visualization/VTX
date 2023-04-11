#ifndef __VTX_UTIL_CHRONO__
#define __VTX_UTIL_CHRONO__

#include <chrono>
#include <string>

namespace VTX::Util
{
	class Chrono
	{
	  public:
		static long long getTimestamp();

		void		start();
		void		stop();
		float		elapsedTime() const;
		std::string elapsedTimeStr() const;
		float		intervalTime();

	  private:
		using SystemClock = std::chrono::system_clock;
		using Clock		  = std::chrono::high_resolution_clock;
		using Duration	  = std::chrono::duration<float>;
		using Ms		  = std::chrono::milliseconds;

		Clock::time_point begin;
		Clock::time_point interval;
		Clock::time_point end;
	};
} // namespace VTX::Util
#endif
