#include "app/core/monitoring/stats.hpp"

namespace VTX::App::Core::Monitoring
{
	Stats::Stats( const unsigned int p_activeFrameCount, const unsigned int p_archivedFrameCount ) :
		_activeFrameCount( p_activeFrameCount ), _archivedFrameCount( p_archivedFrameCount )
	{
	}

	FrameInfo & Stats::newFrame()
	{
		_pushFrame();
		return _activeFrames.back();
	}
	void Stats::_pushFrame()
	{
		_activeFrames.emplace_back( FrameInfo() );

		while ( _activeFrames.size() > _activeFrameCount )
		{
			_archivedFrames.emplace_back( std::move( _activeFrames.front() ) );
			_activeFrames.pop_front();
		}

		while ( _archivedFrames.size() > _archivedFrameCount )
		{
			_archivedFrames.pop_front();
		}
	}

	FrameInfo &		  Stats::getCurrentFrame() { return _activeFrames.back(); }
	const FrameInfo & Stats::getLastFrame() const { return *( ++_activeFrames.rbegin() ); }

	void Stats::clearArchive() { _archivedFrames.clear(); }
	void Stats::clear()
	{
		clearArchive();
		_activeFrames.clear();
	}
} // namespace VTX::App::Core::Monitoring
