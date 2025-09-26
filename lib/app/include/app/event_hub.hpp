#ifndef __VTX_APP_EVENT_HUB__
#define __VTX_APP_EVENT_HUB__

#include "app/ecs.hpp"

namespace VTX::App
{
	class EventHub
	{
	  public:
		using Connection	   = entt::connection;
		using ScopedConnection = entt::scoped_connection;
		using Dispatcher	   = entt::dispatcher;

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
		 * @brief Connect a callable to an event and own its lifetime.
		 */
		template<typename Ev, typename Callable>
		void connect( Callable && p_cb )
		{
			using Fn		  = std::remove_cvref_t<Callable>;
			auto	   holder = std::make_shared<Fn>( std::forward<Callable>( p_cb ) );
			Connection c	  = _bus.sink<Ev>().template connect<&Fn::operator()>( *holder );
			_owned.push_back( Owned { std::move( holder ), std::move( c ) } );
		}

		/**
		 * @brief Connect a callable to an event that will be called only once, then disconnected.
		 */
		template<typename Ev, typename Callable>
		void connectOnce( Callable && p_cb )
		{
			using Fn = std::remove_cvref_t<Callable>;
			struct Once
			{
				Fn		   fn;
				Connection conn;
				void	   operator()( Ev & p_e )
				{
					fn( p_e );
					conn.release();
				}
			};
			auto	   obj = std::make_shared<Once>( Once { std::forward<Callable>( p_cb ), {} } );
			Connection c   = _bus.sink<Ev>().template connect<&Once::operator()>( *obj );
			obj->conn	   = c;
			_owned.push_back( Owned { std::move( obj ), std::move( c ) } );
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
		void trigger( Ev p_ev = {} )
		{
			_bus.trigger<Ev>( std::move( p_ev ) );
		}

		/**
		 * @brief Trigger an event immediately, constructing it in place.
		 */
		template<typename Ev, typename... Args>
		void trigger( Args &&... p_args )
		{
			_bus.trigger<Ev>( std::forward<Args>( p_args )... );
		}

		/**
		 * @brief Enqueue an event to be triggered at the next update.
		 */
		template<typename Ev>
		void enqueue( Ev p_ev = {} )
		{
			_bus.enqueue<Ev>( std::move( p_ev ) );
		}

		/**
		 * @brief Enqueue an event to be triggered at the next update, constructing it in place.
		 */
		template<typename Ev, typename... Args>
		void enqueue( Args &&... p_args )
		{
			_bus.enqueue<Ev>( std::forward<Args>( p_args )... );
		}

		/**
		 * @brief Process all queued events.
		 */
		void update()
		{
			_bus.update();
			sweepOwned();
		}

		/**
		 * @brief Clear all queued events of a given type.
		 */
		template<typename Ev>
		void clear()
		{
			_bus.clear<Ev>();
		}

		/**
		 * @brief Removes elements from the _owned container whose associated connections are not valid or not
		 * connected.
		 */
		void sweepOwned()
		{
			// TOFIX: not deleted.
			_owned.erase(
				std::remove_if( _owned.begin(), _owned.end(), []( const Owned & p_o ) { return not p_o.connexion; } ),
				_owned.end()
			);
		}

		/**
		 * @brief Get the number of currently owned connections.
		 */
		inline size_t ownedConnectionCount() const { return _owned.size(); }

	  private:
		/**
		 * @brief An instance of the entt::dispatcher class used for event dispatching.
		 */
		Dispatcher _bus;

		/**
		 * @brief Represents an owned connection, holding a shared resource and its associated connection object.
		 */
		struct Owned
		{
			std::shared_ptr<void> keepAlive;
			Connection			  connexion;
		};

		/**
		 * @brief A collection of owned connections, ensuring their lifetime is managed appropriately.
		 */
		std::vector<Owned> _owned;
	};

	inline EventHub & HUB() { return ECS::getCtx<EventHub>(); }
} // namespace VTX::App

#endif
