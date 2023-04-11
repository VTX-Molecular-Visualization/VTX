#include "chrono.hpp"

namespace VTX::Util
{
	long long Chrono::getTimestamp()
	{
		SystemClock::time_point now = SystemClock::now();
		Ms						ms	= std::chrono::duration_cast<Ms>( now.time_since_epoch() );
		return ms.count();
	}

	void Chrono::start() { begin = interval = Clock::now(); }

	void Chrono::stop() { end = interval = Clock::now(); }

	float Chrono::elapsedTime() const { return ( std::chrono::duration_cast<Duration>( end - begin ) ).count(); }

	std::string Chrono::elapsedTimeStr() const { return std::to_string( elapsedTime() ) + 's'; }

	float Chrono::intervalTime()
	{
		Duration intervalTime = std::chrono::duration_cast<Duration>( Clock::now() - interval );
		interval			  = Clock::now();
		return ( intervalTime ).count();
	}

} // namespace VTX::Util
