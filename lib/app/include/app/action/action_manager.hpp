#ifndef __VTX_APP_ACTION_ACTION_MANAGER__
#define __VTX_APP_ACTION_ACTION_MANAGER__

#include "app/events.hpp"
#include "app/services.hpp"
#include <atomic>
#include <concepts>
#include <condition_variable>
#include <latch>
#include <memory>
#include <mutex>
#include <util/chrono.hpp>
#include <util/hashing.hpp>
#include <util/logger.hpp>
#include <util/string.hpp>
#include <util/thread/thread_manager.hpp>

namespace VTX::App::Action
{
	/**
	 * @brief Concept for action that can be threadable.
	 */
	template<typename T, typename... Args>
	concept ThreadableAction = requires( T t, Util::Thread::ThreadData thrData, Args &&... args ) {
		{ T( thrData ) };
	};

	class QueuedAction;

	/**
	 * @brief Handle the execution of all actions.
	 */
	class ActionManager
	{
	  public:
		ActionManager();

		void update( const float, const float );
		void shutdown() noexcept;

		/**
		 * @brief Thread safe. Subscribe an action to be executed later on the main thread.
		 * @param
		 */
		void subscribe( QueuedAction ) noexcept;

		/**
		 * @brief Execute an action of type A with the given arguments.
		 */
		template<typename A, typename... Args>
			requires( not ThreadableAction<A, Args...> )
		void execute( Args &&... p_args ) const
		{
			A a;
			execute( a, std::forward<Args>( p_args )... );
		}

		/**
		 * @brief Execute the given action, priorizing multithreading except when there is no gui.
		 */
		template<typename A, typename... Args>
			requires ThreadableAction<A, Args...>
		void execute( Args &&... p_args ) const
		{
			if ( _noThread() )
			{
				A a;
				execute( a, std::forward<Args>( p_args )... );
			}
			else
			{
				THREAD().createThread(
					[... args = std::forward<Args>( p_args ) ](
						Util::Thread::StopToken p_token, Util::Thread::BaseThread & p_thr
					) mutable
					{
						A action( Util::Thread::ThreadData { std::move( p_token ), p_thr } );
						action.execute( std::move( args )... );
						return 0;
					}
				);
			}
		}

		/**
		 * @brief Execute the given action right away.
		 */
		template<typename A, typename... Args>
		void execute( A & p_action, Args &&... p_args ) const
		{
			const auto actionName = Util::typeName<A>();
			VTX_TRACE( "ActionSystem::execute( {} )", actionName );
			try
			{
				auto duration = Util::CHRONO_CPU( [ & ]() { p_action.execute( std::forward<Args>( p_args )... ); } );
				VTX_TRACE(
					"ActionSystem::execute( {} ) - done ({})", actionName, Util::String::durationToStr( duration )
				);
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "{}", p_e.what() );
				return;
			}
		}

	  private:
		struct _Data;

		struct Del
		{
			void operator()( _Data * ) noexcept;
		};

		std::unique_ptr<_Data, Del> _attributesPtr;
		float						_skipTime = 0.f;

		bool _noThread() const noexcept;
	};

	/**
	 * @brief Used to determine if an action can be waited upon.
	 */
	template<typename T>
	concept Waitable = requires( T obj ) {
		{ obj.wait() };
	};

	/**
	 * @brief Meant to be used to subscribe an action to the next main loop update by the QueueAction class object. The
	 * input action can satisfy the Waitable requirement. If it does, the Waiter obtained from the getWaiter method will
	 * call it so the consumer that subscribed the action can wait for it to finish.
	 */
	class QueuedAction
	{
	  public:
		class Waiter;
		QueuedAction();
		~QueuedAction();

		inline void execute() { _ptr->execute(); }

		/**
		 * @brief Returns an object that allow to wait for the action to finish its execution. Please to not wait on the
		 * main thread.
		 * @return
		 */
		Waiter getWaiter();

	  private:
		struct _interface
		{
			virtual ~_interface()  = default;
			virtual void execute() = 0;
			virtual void wait()	   = 0;
		};

		struct _State
		{
			std::mutex				mutex;
			std::condition_variable conditionVariable;
			bool					executed  = false;
			bool					cancelled = false;
		};

		struct _dummy
		{
		};

		template<typename SomeAction, typename... Args>
		class _wrapper final : public _interface
		{
			SomeAction _obj;

			std::tuple<std::remove_reference_t<Args>...> _args;
			std::shared_ptr<_State>						 _state;

		  public:
			_wrapper() = delete;

			_wrapper( std::shared_ptr<_State> p_state, SomeAction && p_action, Args &&... args ) :
				_obj( std::forward<SomeAction>( p_action ) ), _args( std::forward<Args>( args )... ),
				_state( std::move( p_state ) )
			{
			}

			void execute() override
			{
				if constexpr ( not std::same_as<SomeAction, _dummy> )
				{
					auto exec = [ this ]( auto &&... args ) mutable
					{ ACTION().execute<SomeAction>( _obj, std::forward<decltype( args )>( args )... ); };
					std::apply( exec, std::move( _args ) );
				}

				{
					std::scoped_lock<std::mutex> lock( _state->mutex );
					_state->executed = true;
				}
				_state->conditionVariable.notify_all();
			}

			void wait() override
			{
				if constexpr ( Waitable<SomeAction> )
				{
					_obj.wait();
				}
			}
		};

		std::shared_ptr<_State>		_state = std::make_shared<_State>();
		std::shared_ptr<_interface> _ptr;

	  public:
		template<typename SomeAction, typename... Args>
		QueuedAction( SomeAction && p_action, Args &&... args ) :
			_ptr( new _wrapper<SomeAction, Args...>(
				_state,
				std::forward<SomeAction>( p_action ),
				std::forward<Args>( args )...
			) )
		{
		}

		QueuedAction( QueuedAction && )					 = default;
		QueuedAction( const QueuedAction & )			 = delete;
		QueuedAction & operator=( QueuedAction && )		 = default;
		QueuedAction & operator=( const QueuedAction & ) = delete;

		template<typename SomeAction>
		QueuedAction( SomeAction && p_action ) :
			_ptr( new _wrapper<SomeAction>( _state, std::forward<SomeAction>( p_action ) ) )
		{
		}

		/**
		 * @brief Class meant to provide an interface to wait for the action to finish its execution.
		 */
		class Waiter
		{
		  public:
			/**
			 * @brief Stop the execution until the linked action is finished. Please do not wait on the
			 * main thread.
			 */
			inline void wait() noexcept
			{
				bool executed = false;
				{
					_State *					 state = _statePtr.get();
					std::unique_lock<std::mutex> lock( state->mutex );
					state->conditionVariable.wait( lock, [ state ]() { return state->executed || state->cancelled; } );
					executed = state->executed;
				}
				if ( executed )
				{
					this->_ptr->wait();
				}
			}

		  private:
			inline Waiter( std::shared_ptr<_interface> ptr, std::shared_ptr<_State> p_state ) :
				_ptr( ptr ), _statePtr( std::move( p_state ) )
			{
			}

			friend QueuedAction;
			std::shared_ptr<_interface> _ptr;
			std::shared_ptr<_State>		_statePtr;
		};
	};

	/**
	 * @brief Delays an action until a future main loop update. Be mindfull of the arguments used as they will be stored
	 * until the action is executed.
	 * @tparam SomeAction Action to be delayed
	 */
	template<typename SomeAction>
	class QueueAction
	{
	  public:
		template<typename... Args>
		inline void execute( Args &&... args )
		{
			QueuedAction action( SomeAction(), std::forward<Args>( args )... );
			auto		 waiter = action.getWaiter();
			ACTION().subscribe( std::move( action ) );
			waiter.wait();
		}
	};
} // namespace VTX::App::Action

#endif
