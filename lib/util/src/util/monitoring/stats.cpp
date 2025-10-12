#include "util/monitoring/stats.hpp"
#include <util/chrono.hpp>

namespace VTX::Util::Monitoring
{

	FrameInfo & Stats::newFrame()
	{
		_frames.emplace_back( FrameInfo() );

		// Remove old frames
		while ( _frames.size() > ACTIVE_FRAME_COUNT )
		{
			_frames.pop_front();
		}

		return _frames.back();
	}

	float Stats::average() const
	{
		if ( _frames.size() < 2 )
		{
			return 0;
		}

		float res = Util::Chrono::elapsedTime( _frames.front().getTimepoint(), _frames.back().getTimepoint() );

		return res / float( _frames.size() - 1 );
	}
} // namespace VTX::Util::Monitoring
