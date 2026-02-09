#include "app/pass/action_executer.hpp"
#include "app/action/camera.hpp"
#include "app/ecs.hpp"
#include "app/services.hpp"
#include <mutex>
#include <queue>
namespace VTX::App::Pass
{
	namespace
	{

		struct ActionQueue
		{
			std::queue<QueuedAction> actions;
			std::mutex				 mutex;
		};
		struct DelayFunctionsQueue
		{
			std::shared_ptr<std::latch> latch;
		};

	} // namespace
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
	std::shared_ptr<std::latch> getWaitTicket()
	{
		for ( auto entity : REG().view<DelayFunctionsQueue>() )
		{
			std::shared_ptr<std::latch> out;
			REG().patch<DelayFunctionsQueue>(
				entity,
				[ &out ]( DelayFunctionsQueue & p )
				{
					if ( p.latch == nullptr )
						p.latch = std::make_shared<std::latch>( 1 );
					out = p.latch;
				}
			);
			return out;
		}
		return std::shared_ptr<std::latch>( new std::latch( 1 ) );
	}

	ActionExecuter::ActionExecuter() : _queueEntity( REG().create() )
	{
		REG().emplace<ActionQueue>( _queueEntity );
		REG().emplace<DelayFunctionsQueue>( _queueEntity );
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

				while ( not p_queueObj.actions.empty() )
				{
					QueuedAction & action = p_queueObj.actions.front();
					action.execute();
					p_queueObj.actions.pop();
				}
			}
		);
		REG().patch<DelayFunctionsQueue>(
			_queueEntity,
			[]( DelayFunctionsQueue & p )
			{
				if (p.latch)
				{
					p.latch->count_down();
					p.latch = nullptr;
				}
			}
		);
	}

} // namespace VTX::App::Pass
