#ifndef VTX_APP_ACTION_ACTION_MANAGER_HPP
#define VTX_APP_ACTION_ACTION_MANAGER_HPP

#include "app/services.hpp"
#include <atomic>
#include <concepts>
#include <condition_variable>
#include <exception>
#include <latch>
#include <memory>
#include <mutex>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <util/chrono.hpp>
#include <util/fmt/duration.hpp>
#include <util/hashing.hpp>
#include <util/logger.hpp>
#include <util/string.hpp>
#include <util/thread/thread_manager.hpp>
#include <utility>

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
		/**
		 * @brief Constructor.
		 */
		ActionManager();

		/**
		 * @brief Handle queued actions.
		 */
		void update( float, float );

		/**
		 * @brief Clear pending actions.
		 */
		void shutdown();

		/**
		 * @brief Thread safe. Subscribe an action to be executed later on the main thread.
		 * @param
		 */
		void subscribe( QueuedAction );

		/**
		 * @brief Execute an action of type A with the given arguments.
		 */
		template<typename A, typename... Args>
			requires( not ThreadableAction<A, Args...> )
		void execute( Args &&... p_args ) const noexcept
		{
			_guard(
				Util::typeName<A>(),
				[ & ]()
				{
					A a;
					_run( a, std::forward<Args>( p_args )... );
				}
			);
		}

		/**
		 * @brief Execute the given action, priorizing multithreading except when there is no gui.
		 */
		template<typename A, typename... Args>
			requires ThreadableAction<A, Args...>
		void execute( Args &&... p_args ) const noexcept
		{
			if ( _noThread() )
			{
				_guard(
					Util::typeName<A>(),
					[ & ]()
					{
						A a;
						_run( a, std::forward<Args>( p_args )... );
					}
				);
			}
			else
			{
				_guard(
					Util::typeName<A>(),
					[ & ]()
					{
						THREAD().createThread(
							[... args = std::forward<Args>( p_args ) ](
								Util::Thread::StopToken p_token, Util::Thread::BaseThread & p_thr
							) mutable
							{
								const bool succeeded = _guard(
									Util::typeName<A>(),
									[ & ]()
									{
										A action( Util::Thread::ThreadData { std::move( p_token ), p_thr } );
										action.execute( std::move( args )... );
									}
								);
								if ( not succeeded )
								{
									p_thr.setFailed();
								}
								return 0;
							}
						);
					}
				);
			}
		}

		/**
		 * @brief Execute the given action right away.
		 */
		template<typename A, typename... Args>
		void execute( A & p_action, Args &&... p_args ) const noexcept
		{
			_guard( Util::typeName<A>(), [ & ]() { _run( p_action, std::forward<Args>( p_args )... ); } );
		}

	  private:
		friend class QueuedAction;
		struct Data;

		struct Del
		{
			void operator()( Data * ) noexcept;
		};

		std::unique_ptr<Data, Del> _attributesPtr;
		float					   _skipTime = 0.f;

		bool _noThread() const noexcept;

		template<typename A, typename... Args>
		static void _run( A & p_action, Args &&... p_args )
		{
			const auto actionName = Util::typeName<A>();
			VTX_TRACE( "ActionSystem::execute( {} )", actionName );
			auto duration = Util::CHRONO_CPU( [ & ]() { p_action.execute( std::forward<Args>( p_args )... ); } );
			VTX_TRACE(
				"ActionSystem::execute( {} ) - done ({})", actionName, Util::String::FormattedDuration { duration }
			);
		}

		template<typename F>
		static bool _guard( const std::string_view p_actionName, F && p_function ) noexcept
		{
			try
			{
				std::forward<F>( p_function )();
				return true;
			}
			catch ( ... )
			{
				_logFailure( p_actionName, std::current_exception() );
			}
			return false;
		}

		static void _logFailure( const std::string_view p_actionName, const std::exception_ptr & p_error ) noexcept
		{
			try
			{
				std::rethrow_exception( p_error );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Action {} failed: {}", p_actionName, p_e.what() );
			}
			catch ( ... )
			{
				VTX_ERROR( "Action {} failed: unknown exception", p_actionName );
			}
		}
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

		void execute() { _ptr->execute(); }

		/**
		 * @brief Returns an object that allow to wait for the action to finish its execution. Please to not wait on the
		 * main thread.
		 * @return
		 */
		Waiter getWaiter() noexcept;

	  private:
		struct _interface
		{
			virtual ~_interface()  = default;
			virtual void execute() = 0;
			virtual void wait()	   = 0;
		};

		struct State
		{
			std::mutex				mutex;
			std::condition_variable conditionVariable;
			bool					executed  = false;
			bool					cancelled = false;
			std::exception_ptr		error;
		};

		struct _dummy
		{
		};

		template<typename SomeAction, typename... Args>
		class _wrapper final : public _interface
		{
			SomeAction _obj;

			std::tuple<std::remove_cvref_t<Args>...> _args;
			std::shared_ptr<State>					 _state;

		  public:
			_wrapper() = delete;

			_wrapper( std::shared_ptr<State> p_state, SomeAction && p_action, Args &&... args ) :
				_obj( std::forward<SomeAction>( p_action ) ), _args( std::forward<Args>( args )... ),
				_state( std::move( p_state ) )
			{
			}

			void execute() override
			{
				std::exception_ptr error;
				if constexpr ( not std::same_as<SomeAction, _dummy> )
				{
					try
					{
						auto exec = [ this ]( auto &&... args ) mutable
						{ ActionManager::_run( _obj, std::forward<decltype( args )>( args )... ); };
						std::apply( exec, std::move( _args ) );
					}
					catch ( ... )
					{
						error = std::current_exception();
						ActionManager::_logFailure( Util::typeName<SomeAction>(), error );
					}
				}

				{
					const std::scoped_lock<std::mutex> lock( _state->mutex );
					_state->error	 = std::move( error );
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

		std::shared_ptr<State>		_state = std::make_shared<State>();
		std::shared_ptr<_interface> _ptr;

	  public:
		template<typename SomeAction, typename... Args>
			requires( not std::same_as<std::remove_cvref_t<SomeAction>, QueuedAction> )
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
			requires( not std::same_as<std::remove_cvref_t<SomeAction>, QueuedAction> )
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
			void wait()
			{
				bool			   executed = false;
				std::exception_ptr error;
				{
					State *						 state = _statePtr.get();
					std::unique_lock<std::mutex> lock( state->mutex );
					state->conditionVariable.wait( lock, [ state ]() { return state->executed || state->cancelled; } );
					executed = state->executed;
					error	 = state->error;
				}
				if ( error )
				{
					std::rethrow_exception( error );
				}
				if ( executed )
				{
					this->_ptr->wait();
				}
			}

		  private:
			Waiter( std::shared_ptr<_interface> ptr, std::shared_ptr<State> p_state ) noexcept :
				_ptr( std::move( ptr ) ), _statePtr( std::move( p_state ) )
			{
			}

			friend QueuedAction;
			std::shared_ptr<_interface> _ptr;
			std::shared_ptr<State>		_statePtr;
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
		void execute( Args &&... args )
		{
			QueuedAction action( SomeAction(), std::forward<Args>( args )... );
			auto		 waiter = action.getWaiter();
			ACTION().subscribe( std::move( action ) );
			waiter.wait();
		}
	};
} // namespace VTX::App::Action

#endif
