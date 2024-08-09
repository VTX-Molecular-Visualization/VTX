#include "app/core/monitoring/stats.hpp"
#include "app/internal/monitoring/all_metrics.hpp"

namespace VTX::App::Core::Monitoring
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

	// const FrameInfo & Stats::getLastFrame() const { return *( ++_activeFrames.rbegin() ); }

	// void Stats::clearArchive() { _archivedFrames.clear(); }
	// void Stats::clear()
	//{
	//	clearArchive();
	//	_activeFrames.clear();
	//}
} // namespace VTX::App::Core::Monitoring
