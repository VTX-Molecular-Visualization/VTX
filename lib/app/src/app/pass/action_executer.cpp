#include "app/pass/action_executer.hpp"
#include "app/action/camera.hpp"
#include "app/ecs.hpp"
#include "app/services.hpp"
#include <mutex>
#include <queue>
namespace VTX::App::Pass
{

	struct ActionQueue
	{
		std::queue<QueuedAction> actions;
		std::mutex				 mutex;
	};

	void subscribe( QueuedAction p_action ) noexcept
	{
		for ( auto entity : REG().view<ActionQueue>() )
		{
			REG().patch<ActionQueue>(
				entity,
				[ &p_action ]( ActionQueue & p_queueObj )
				{
					std::scoped_lock<std::mutex> guard( p_queueObj.mutex );
					p_queueObj.actions.push( std::move( p_action ) );
				}
			);
		}
	}

	ActionExecuter::ActionExecuter() : _queueEntity( REG().create() )
	{
		//
		REG().emplace<ActionQueue>( _queueEntity );
		//
	}
	void ActionExecuter::update( const float p_delta, const float p_elapsedTime )
	{
		_skipTime += p_delta;
		if ( _skipTime < 100.f )
		{ // We do not want to do that every frame.
			return;
		}
		_skipTime = 0.f;

		REG().patch<ActionQueue>(
			_queueEntity,
			[]( ActionQueue & p_queueObj )
			{
				std::scoped_lock<std::mutex> guard( p_queueObj.mutex );
				if ( p_queueObj.actions.empty() )
					return;

				QueuedAction & action = p_queueObj.actions.front();
				action.execute();
				p_queueObj.actions.pop();
			}
		);
	}

} // namespace VTX::App::Pass
