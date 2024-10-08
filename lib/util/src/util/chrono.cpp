#include "util/chrono.hpp"

namespace VTX::Util
{
	long long Chrono::getTimestamp()
	{
		SystemClock::time_point now = SystemClock::now();
		Ms						ms	= std::chrono::duration_cast<Ms>( now.time_since_epoch() );
		return ms.count();
	}

	void Chrono::start() { _begin = _interval = Clock::now(); }

	void Chrono::stop() { _end = _interval = Clock::now(); }

	float Chrono::elapsedTime() const
	{
		return ( std::chrono::duration_cast<Duration>( Clock::now() - _begin ) ).count();
	}

	float Chrono::intervalTime()
	{
		Duration intervalTime = std::chrono::duration_cast<Duration>( Clock::now() - _interval );
		_interval			  = Clock::now();
		return intervalTime.count();
	}

} // namespace VTX::Util
