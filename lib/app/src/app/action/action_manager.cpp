#include "app/action/action_manager.hpp"
#include "app/arguments.hpp"
#include "app/services.hpp"
#include <mutex>
#include <queue>

namespace VTX::App::Action
{

	struct ActionManager::Data
	{
		bool					 noThread	  = false;
		std::atomic_bool		 shuttingDown = false;
		std::thread::id			 mainThreadId = std::this_thread::get_id();
		std::queue<QueuedAction> actions;
		std::mutex				 mutex;
	};

	void ActionManager::Del::operator()( ActionManager::Data * const p_ ) noexcept
	{
		// NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
		delete p_;
	}

	ActionManager::ActionManager() : _attributesPtr( new Data { ARGS().noGui } ) {}

	QueuedAction::QueuedAction() : _ptr( new _wrapper<_dummy>( _state, _dummy() ) ) {}

	QueuedAction::~QueuedAction()
	{
		if ( _state )
		{
			const std::scoped_lock<std::mutex> lock( _state->mutex );
			_state->cancelled = true;
			_state->conditionVariable.notify_all();
		}
	}

	void ActionManager::update( const float p_delta, const float )
	{
		_skipTime += p_delta;
		if ( _skipTime < 100.f )
		{
			// We do not want to do that every frame.
			// TODO: why not?
			return;
		}
		const std::scoped_lock<std::mutex> guard( _attributesPtr->mutex );
		if ( _attributesPtr->actions.empty() )
		{
			return;
		}

		while ( not _attributesPtr->actions.empty() )
		{
			QueuedAction & action = _attributesPtr->actions.front();
			action.execute();
			_attributesPtr->actions.pop();
		}
	};

	void ActionManager::subscribe( QueuedAction p_action )
	{
		if ( _noThread() || _attributesPtr->mainThreadId == std::this_thread::get_id() )
		{
			if ( _attributesPtr->shuttingDown )
			{
				return;
			}
			p_action.execute();
			return;
		}
		const std::scoped_lock<std::mutex> guard( _attributesPtr->mutex );
		if ( _attributesPtr->shuttingDown )
		{
			return;
		}
		_attributesPtr->actions.push( std::move( p_action ) );
	}

	void ActionManager::shutdown()
	{
		_attributesPtr->shuttingDown = true;
		std::queue<QueuedAction> actions;
		{
			const std::scoped_lock<std::mutex> guard( _attributesPtr->mutex );
			std::swap( actions, _attributesPtr->actions );
		}
	}

	bool ActionManager::_noThread() const noexcept { return _attributesPtr->noThread; }

	QueuedAction::Waiter QueuedAction::getWaiter() { return Waiter( _ptr, _state ); }
} // namespace VTX::App::Action
