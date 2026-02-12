
#include "app/action/action_manager.hpp"
#include "app/events.hpp"
#include "app/services.hpp"
#include <mutex>
#include <queue>
#include <util/event_hub.hpp>

namespace VTX::App::Action
{

	struct ActionManager::_Data
	{
		std::queue<QueuedAction> actions;
		std::mutex				 mutex;
	};
	void ActionManager::Del::operator()( ActionManager::_Data * p_ ) noexcept { delete p_; }

	ActionManager::ActionManager() : _attributesPtr( new _Data() ) {}

	void ActionManager::update( const float p_delta, const float ) noexcept
	{
		_skipTime += p_delta;
		if ( _skipTime < 100.f )
		{ // We do not want to do that every frame.
			return;
		}
		std::scoped_lock<std::mutex> guard( _attributesPtr->mutex );
		if ( _attributesPtr->actions.empty() )
			return;

		while ( not _attributesPtr->actions.empty() )
		{
			QueuedAction & action = _attributesPtr->actions.front();
			action.execute();
			_attributesPtr->actions.pop();
		}
	};

	void ActionManager::subscribe( QueuedAction p_action ) noexcept
	{
		std::scoped_lock<std::mutex> guard( _attributesPtr->mutex );
		_attributesPtr->actions.push( std::move( p_action ) );
	}
	QueuedAction::Waiter QueuedAction::getWaiter() { return Waiter( _ptr ); }
} // namespace VTX::App::Action
