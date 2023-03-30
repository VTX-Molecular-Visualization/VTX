#ifndef __VTX_UTIL_CHRONO__
#define __VTX_UTIL_CHRONO__

#include <chrono>
#include <string>

namespace VTX::Util
{

	class Chrono
	{
		using SystemClock = std::chrono::system_clock;
		using Clock		  = std::chrono::high_resolution_clock;
		using Duration	  = std::chrono::duration<float>;
		using Ms		  = std::chrono::milliseconds;

	  public:
		inline void	 start() { begin = interval = Clock::now(); }
		inline void	 stop() { end = interval = Clock::now(); }
		inline float elapsedTime() const { return ( std::chrono::duration_cast<Duration>( end - begin ) ).count(); }
		inline std::string elapsedTimeStr() const { return std::to_string( elapsedTime() ) + 's'; }

		float intervalTime()
		{
			Duration intervalTime = std::chrono::duration_cast<Duration>( Clock::now() - interval );
			interval			  = Clock::now();
			return ( intervalTime ).count();
		}

		// inline static std::string getNowString() { return ""; }

		static long long getTimestamp()
		{
			SystemClock::time_point now = SystemClock::now();
			Ms						ms	= std::chrono::duration_cast<Ms>( now.time_since_epoch() );
			return ms.count();
		}

	  private:
		Clock::time_point begin;
		Clock::time_point interval;
		Clock::time_point end;
	};

} // namespace VTX::Util
#endif
