
#include "util/history_browser.hpp"

namespace VTX::Util
{
	void HistoryBrowser::add( std::string p_newHistory ) noexcept { _history.push_back( std::move( p_newHistory ) ); }

	void HistoryBrowser::tryMoveForward() noexcept
	{
		if ( _browsingHistory )
		{
			_historyIdx = _historyIdx == 0 ? 0 : _historyIdx - 1;
		}
	}
	void HistoryBrowser::tryMoveBackward() noexcept
	{
		if ( _browsingHistory )
		{
			_historyIdx = std::min( _history.size() - 1, _historyIdx + 1 );
		}
		_browsingHistory = true;
	}
	void HistoryBrowser::resetBrowsing() noexcept
	{
		_historyIdx		 = 0;
		_browsingHistory = false;
	}
	std::string HistoryBrowser::currentString() const noexcept
	{
		if ( _browsingHistory )
			return _history[ _history.size() - 1 - _historyIdx ];
		return {};
	}

} // namespace VTX::Util
