#ifndef __VTX_UTIL_EVENT_HUB__
#define __VTX_UTIL_EVENT_HUB__

#include <entt/entt.hpp>
#include <mutex>
#include <utility>

namespace VTX::Util
{
	/**
	 * @brief A centralized event management system using the EnTT library.
	 */
	class EventHub
	{
	  public:
		using Connection	   = entt::connection;
		using ScopedConnection = entt::scoped_connection;
		using Dispatcher	   = entt::dispatcher;
		template<typename... Args>
		using Delegate = entt::delegate<Args...>;

		/**
		 * @brief Connect a function to an event.
		 */
		template<typename Ev, auto Fn, typename... Args>
		Connection connect( Args &&... p_args )
		{ return _bus.sink<Ev>().template connect<Fn>( std::forward<Args>( p_args )... ); }

		/**
		 * @brief Connect a callable to an event.
		 */
		template<typename Ev, typename Callable>
		Connection connect( Callable & p_cb )
		{
			using Fn = std::remove_cvref_t<Callable>;
			return _bus.sink<Ev>().template connect<&Fn::operator()>( p_cb );
		}

		/**
		 * @brief Disconnect a connection.
		 */
		static void disconnect( Connection & p_c ) { p_c.release(); }

		/**
		 * @brief Disconnect all connections of an instance.
		 */
		template<typename Obj>
		void disconnectAllOf( Obj & p_obj )
		{ _bus.disconnect( p_obj ); }

		/**
		 * @brief Disconnect all listeners of a given event type.
		 */
		template<typename Ev>
		void disconnectAllListenersOf()
		{ _bus.sink<Ev>().disconnect(); }

		/**
		 * @brief Trigger an event immediately.
		 */
		template<typename Ev>
		void trigger( Ev p_ev )
		{ _bus.trigger<Ev>( std::move( p_ev ) ); }

		/**
		 * @brief Trigger an event immediately, constructing it in place.
		 */
		template<typename Ev, typename... Args>
		void trigger( Args &&... p_args )
		{ _bus.trigger<Ev>( Ev( std::forward<Args>( p_args )... ) ); }

		/**
		 * @brief Enqueue an event to be triggered at the next update (thread-safe).
		 */
		template<typename Ev>
		void enqueue( Ev p_ev )
		{
			const std::scoped_lock lock( _pendingMutex );
			auto				   sink = _pending.sink<Ev>();
			if ( sink.empty() )
			{
				sink.template connect<&EventHub::_forward<Ev>>( this );
			}
			_pending.enqueue<Ev>( std::move( p_ev ) );
		}

		/**
		 * @brief Enqueue an event to be triggered at the next update, constructing it in place (thread-safe).
		 */
		template<typename Ev, typename... Args>
		void enqueue( Args &&... p_args )
		{ enqueue<Ev>( Ev( std::forward<Args>( p_args )... ) ); }

		/**
		 * @brief Process all queued events (thread-safe).
		 */
		void update()
		{
			Dispatcher pending;
			{
				const std::scoped_lock lock( _pendingMutex );
				pending.swap( _pending );
			}
			pending.update();
			_bus.update();
		}

		/**
		 * @brief Clear all queued events of a given type.
		 */
		template<typename Ev>
		void clear()
		{
			{
				const std::scoped_lock lock( _pendingMutex );
				_pending.clear<Ev>();
			}
			_bus.clear<Ev>();
		}

	  private:
		/**
		 * @brief Forward an event from the pending queue to the main dispatcher.
		 */
		template<typename Ev>
		void _forward( Ev & p_event )
		{ _bus.enqueue<Ev>( std::move( p_event ) ); }

		/**
		 * @brief A mutex to protect access to the pending event queue.
		 */
		std::mutex _pendingMutex;

		/**
		 * @brief An instance of the entt::dispatcher class used for pending events.
		 */
		Dispatcher _pending;

		/**
		 * @brief An instance of the entt::dispatcher class used for event dispatching.
		 */
		Dispatcher _bus;
	};
} // namespace VTX::Util

#endif
