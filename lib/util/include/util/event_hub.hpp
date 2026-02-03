#ifndef __VTX_UTIL_EVENT_HUB__
#define __VTX_UTIL_EVENT_HUB__

#include <entt/entt.hpp>

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
		{
			return _bus.sink<Ev>().template connect<Fn>( std::forward<Args>( p_args )... );
		}

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
		{
			_bus.disconnect( p_obj );
		}

		/**
		 * @brief Disconnect all listeners of a given event type.
		 */
		template<typename Ev>
		void disconnectAllListenersOf()
		{
			_bus.sink<Ev>().disconnect();
		}

		/**
		 * @brief Trigger an event immediately.
		 */
		template<typename Ev>
		void trigger( Ev p_ev )
		{
			_bus.trigger<Ev>( std::move( p_ev ) );
		}

		/**
		 * @brief Trigger an event immediately, constructing it in place.
		 */
		template<typename Ev, typename... Args>
		void trigger( Args &&... p_args )
		{
			_bus.trigger<Ev>( Ev( std::forward<Args>( p_args )... ) );
		}

		/**
		 * @brief Enqueue an event to be triggered at the next update.
		 */
		template<typename Ev>
		void enqueue( Ev p_ev )
		{
			_bus.enqueue<Ev>( std::move( p_ev ) );
		}

		/**
		 * @brief Enqueue an event to be triggered at the next update, constructing it in place.
		 */
		template<typename Ev, typename... Args>
		void enqueue( Args &&... p_args )
		{
			_bus.enqueue<Ev>( Ev( std::forward<Args>( p_args )... ) );
		}

		/**
		 * @brief Process all queued events.
		 */
		void update() { _bus.update(); }

		/**
		 * @brief Clear all queued events of a given type.
		 */
		template<typename Ev>
		void clear()
		{
			_bus.clear<Ev>();
		}

	  private:
		/**
		 * @brief An instance of the entt::dispatcher class used for event dispatching.
		 */
		Dispatcher _bus;
	};
} // namespace VTX::Util

#endif
