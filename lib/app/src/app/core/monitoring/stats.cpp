#include "app/core/monitoring/stats.hpp"
#include "app/internal/monitoring/all_metrics.hpp"

namespace VTX::App::Core::Monitoring
{

	FrameInfo & Stats::newFrame()
	{
		_pushFrame();
		return _activeFrames.back();
	}

	void Stats::_pushFrame()
	{
		_activeFrames.emplace_back( FrameInfo() );

		while ( _activeFrames.size() > ACTIVE_FRAME_COUNT )
		{
			_activeFrames.pop_front();
		}
	}

	// const FrameInfo & Stats::getLastFrame() const { return *( ++_activeFrames.rbegin() ); }

	// void Stats::clearArchive() { _archivedFrames.clear(); }
	// void Stats::clear()
	//{
	//	clearArchive();
	//	_activeFrames.clear();
	//}
} // namespace VTX::App::Core::Monitoring
