#include "util/chrono.hpp"

namespace VTX::Util
{
	long long Chrono::getTimestamp() noexcept
	{
		SystemClock::time_point now = SystemClock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>( now.time_since_epoch() ).count();
	}

	void Chrono::start() noexcept { _begin = _interval = Clock::now(); }

	void Chrono::stop() noexcept { _end = _interval = Clock::now(); }

	float Chrono::elapsedTime() const noexcept
	{
		return std::chrono::duration_cast<Duration>( Clock::now() - _begin ).count();
	}

	float Chrono::intervalTime() noexcept
	{
		Duration intervalTime = std::chrono::duration_cast<Duration>( Clock::now() - _interval );
		_interval			  = Clock::now();
		return intervalTime.count();
	}

} // namespace VTX::Util
