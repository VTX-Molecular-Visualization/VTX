#include "app/action/action_manager.hpp"
#include "app/args.hpp"
#include "app/events.hpp"
#include "app/services.hpp"
#include "app/vtx_app.hpp"
#include <mutex>
#include <queue>
#include <util/event_hub.hpp>

namespace VTX::App::Action
{

	struct ActionManager::_Data
	{
		bool					 noThread	  = false;
		std::thread::id			 mainThreadId = std::this_thread::get_id();
		std::queue<QueuedAction> actions;
		std::mutex				 mutex;
	};
	void ActionManager::Del::operator()( ActionManager::_Data * p_ ) noexcept { delete p_; }

	ActionManager::ActionManager() : _attributesPtr( new _Data { ARGS().has( App::ARG_NO_GUI ) } ) {}

	void ActionManager::update( const float p_delta, const float ) 
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
		if ( _attributesPtr->mainThreadId == std::this_thread::get_id() )
		{
			p_action.execute();
			return;
		}
		std::scoped_lock<std::mutex> guard( _attributesPtr->mutex );
		_attributesPtr->actions.push( std::move( p_action ) );
	}
	bool ActionManager::_noThread() const noexcept { return _attributesPtr->noThread; }

	QueuedAction::Waiter QueuedAction::getWaiter() { return Waiter( _ptr ); }
} // namespace VTX::App::Action
