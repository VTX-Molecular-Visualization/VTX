#ifndef __VTX_APP_PASS_ACTION_EXECUTER__
#define __VTX_APP_PASS_ACTION_EXECUTER__

#include "app/ecs.hpp"
#include "app/pass/pass_manager.hpp"
#include <functional>
#include <latch>
#include <tuple>

/*
This header introduces 2 concepts : Queued actions and Delayed Callable.
Queued actions are not executed right away but rather saved for execution by the pass, on the main update phase.
Dalayed Callable are run direclty when called but they will wait for the action executer to update before the actual
callable is executed.

This is meant to be a sync mechanisme for threaded execution. To illustrate, lets consider a sequence of python commands
:

Open("3j3q.bcif")
AssociateTrajectory("3j3q.xtc", getSystemId("3j3q"))



*/

namespace VTX::App::Pass
{
	class QueuedAction;

	/**
	 * @brief Subscribe an action to be executed later on the main loop. Meant to be used only through QueueAction
	 * @param
	 */
	void subscribe( QueuedAction ) noexcept;

	/**
	 * @brief Get a ticket to wait until every Queued Actions are executed.
	 * @return
	 */
	std::shared_ptr<std::latch> getWaitTicket();

	/**
	 * @brief Delays an action until a future pass update, in the main loop.
	 * @tparam SomeAction Action to be delayed
	 */
	template<typename SomeAction>
	class QueueAction
	{
	  public:
		template<typename... Args>
		inline void execute( Args &&... args )
		{
			subscribe( QueuedAction( SomeAction(), std::forward<Args>( args )... ) );
		}
	};

	/**
	 * @brief Wrap a function pointer to wait for every Queued Action before invoking the function.
	 * @tparam T
	 * @tparam ...Args
	 * @param p_funcPtr
	 * @return
	 */
	template<typename T, typename... Args>
	std::function<T( Args... )> wrapDelayedFunction( T ( *p_funcPtr )( Args... ) )
	{
		return [ func = p_funcPtr ]( Args... p_args )
		{
			auto waitForIt = getWaitTicket();
			waitForIt->wait();
			return ( *func )( p_args... );
		};
	}

	/**
	 * @brief Pass that execute actions in queue and execute delayed actions afterward. Currently not triggering action
	 * every update for performances reason but maybe in the future we will need to change that idk
	 */
	class ActionExecuter : public IPass
	{
	  public:
		ActionExecuter();

		void update( const float, const float );

	  private:
		ECS::Entity _queueEntity = entt::null;
		float		_skipTime	 = 0.f;
	};

	/**
	 * @brief Meant to be used to subscribe an action to the next main loop update by the QueueAction class object
	 */
	class QueuedAction
	{
	  public:
		QueuedAction() = default;
		inline void execute() { _ptr->execute(); }

	  private:
		struct _interface
		{
			virtual ~_interface()  = default;
			virtual void execute() = 0;
		};
		struct _dummy
		{
		};
		template<typename SomeAction, typename... Args>
		class _wrapper final : public _interface
		{
			SomeAction _obj;

			std::tuple<std::remove_reference_t<Args>...> _args;

		  public:
			_wrapper() = delete;
			_wrapper( SomeAction && p_action, Args &&... args ) :
				_obj( std::forward<SomeAction>( p_action ) ), _args( std::forward<Args>( args )... )
			{
			}
			void execute() override
			{
				if constexpr ( not std::same_as<SomeAction, _dummy> )
				{
					auto exec = [ this ]( auto... args ) { _obj.execute( args... ); };
					std::apply( exec, _args );
				}
			}
		};
		std::unique_ptr<_interface> _ptr = std::make_unique<_wrapper<_dummy>>( _dummy() );

	  public:
		template<typename SomeAction, typename... Args>
		QueuedAction( SomeAction && p_action, Args &&... args ) :
			_ptr(
				new _wrapper<SomeAction, Args...>( std::forward<SomeAction>( p_action ), std::forward<Args>( args )... )
			)
		{
		}
		template<typename SomeAction>
		QueuedAction( SomeAction && p_action ) :
			_ptr( new _wrapper<SomeAction>( std::forward<SomeAction>( p_action ) ) )
		{
		}
	};
} // namespace VTX::App::Pass

#endif
