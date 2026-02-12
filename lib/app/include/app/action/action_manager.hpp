#ifndef __VTX_APP_ACTION_ACTION_MANAGER__
#define __VTX_APP_ACTION_ACTION_MANAGER__

#include "app/events.hpp"
#include "app/services.hpp"
#include <concepts>
#include <latch>
#include <memory>
#include <util/chrono.hpp>
#include <util/hashing.hpp>
#include <util/logger.hpp>
#include <util/string.hpp>

namespace VTX::App::Action
{

	class QueuedAction;

	/**
	 * @brief Handle the execution of all actions.
	 */
	class ActionManager
	{
	  public:
		ActionManager();

		void update( const float, const float ) noexcept;

		/**
		 * @brief Subscribe an action to be executed later on the main loop. Meant to be used only through QueueAction
		 * @param
		 */
		void subscribe( QueuedAction ) noexcept;

		/**
		 * @brief Execute an action of type A with the given arguments.
		 */
		template<typename A, typename... Args>
		void execute( Args &&... p_args ) const
		{
			A a;
			execute( a, std::forward<Args>( p_args )... );
		}

		/**
		 * @brief Execute the given action.
		 */
		template<typename A, typename... Args>
		void execute( A & p_action, Args &&... p_args ) const
		{
			const auto actionName = Util::typeName<A>();
			VTX_DEBUG( "ActionSystem::execute( {} )", actionName );
			try
			{
				auto duration = Util::CHRONO_CPU( [ & ]() { p_action.execute( std::forward<Args>( p_args )... ); } );
				VTX_DEBUG(
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
	};

	template<typename T>
	concept Waitable = requires( T obj ) {
		{ obj.wait() };
	};
	/**
	 * @brief Meant to be used to subscribe an action to the next main loop update by the QueueAction class object
	 */
	class QueuedAction
	{
	  public:
		class Waiter;
		QueuedAction() = default;
		inline void execute() { _ptr->execute(); }
		Waiter		getWaiter();

	  private:
		struct _interface
		{
			virtual ~_interface()  = default;
			virtual void execute() = 0;
			virtual void wait()	   = 0;
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
			void wait() override
			{
				if constexpr ( Waitable<SomeAction> )
					_obj.wait();
			}
		};
		std::shared_ptr<_interface> _ptr = std::make_shared<_wrapper<_dummy>>( _dummy() );

	  public:
		template<typename SomeAction, typename... Args>
		QueuedAction( SomeAction && p_action, Args &&... args ) :
			_ptr(
				new _wrapper<SomeAction, Args...>( std::forward<SomeAction>( p_action ), std::forward<Args>( args )... )
			)
		{
		}
		~QueuedAction()									 = default;
		QueuedAction( QueuedAction && )					 = default;
		QueuedAction( const QueuedAction & )			 = delete;
		QueuedAction & operator=( QueuedAction && )		 = default;
		QueuedAction & operator=( const QueuedAction & ) = delete;
		template<typename SomeAction>
		QueuedAction( SomeAction && p_action ) :
			_ptr( new _wrapper<SomeAction>( std::forward<SomeAction>( p_action ) ) )
		{
		}
		class Waiter
		{
		  public:
			inline void wait() noexcept { _ptr->wait(); }
			inline Waiter( std::shared_ptr<_interface> ptr ) : _ptr( ptr ) {}

		  private:
			std::shared_ptr<_interface> _ptr;
		};
	};

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
			QueuedAction action( SomeAction(), std::forward<Args>( args )... );
			auto		 waiter = action.getWaiter();
			ACTION().subscribe( std::move( action ) );
			waiter.wait();
		}
	};
} // namespace VTX::App::Action

#endif
