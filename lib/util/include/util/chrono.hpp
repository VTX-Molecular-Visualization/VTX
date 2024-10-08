#ifndef __VTX_UTIL_CHRONO__
#define __VTX_UTIL_CHRONO__

#include <chrono>
#include <functional>
#include <string>

namespace VTX::Util
{
	class Chrono
	{
	  public:
		using Task	= std::function<void()>;
		using Timer = std::function<float( const Chrono::Task & )>;

		static long long getTimestamp();

		void  start();
		void  stop();
		float elapsedTime() const;
		float intervalTime();

	  private:
		using SystemClock = std::chrono::system_clock;
		using Clock		  = std::chrono::high_resolution_clock;
		using Duration	  = std::chrono::duration<float>;
		using Ms		  = std::chrono::milliseconds;

		Clock::time_point _begin;
		Clock::time_point _interval;
		Clock::time_point _end;
	};

	inline const Chrono::Timer CHRONO_CPU = []( const Chrono::Task & p_task )
	{
		Chrono c;
		c.start();
		p_task();
		c.stop();
		return c.elapsedTime();
	};
} // namespace VTX::Util
#endif
