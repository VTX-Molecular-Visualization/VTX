#ifndef __VTX_APP_THREADING_TRIGGER_EVENT__
#define __VTX_APP_THREADING_TRIGGER_EVENT__

#include <app/action/action_manager.hpp>
#include <app/services.hpp>
#include <util/event_hub.hpp>

namespace VTX::App::Threading
{
	/**
	 * @brief Class whose purpose is to tigger the event on the main loop, on the main thread.
	 * @tparam EventType
	 */
	class TiggerEvent
	{
		struct _interface
		{
			~_interface()		   = default;
			virtual void execute() = 0;
		};

		template<typename T>
		struct _wrapper final : public _interface
		{
			T _obj;

			_wrapper( T && p_ ) : _obj( std::forward<T>( p_ ) ) {}

			virtual void execute() override { App::HUB().trigger( std::move( _obj ) ); }
		};

		struct _Action
		{
			_Action( std::unique_ptr<_interface> p_ ) : _ptr( std::move( p_ ) ) {}

			std::unique_ptr<_interface> _ptr;

			inline void execute()
			{
				if ( _ptr )
				{
					_ptr->execute();
				}
			}
		};

	  public:
		template<typename EventType>
		TiggerEvent( EventType && p_event )
		{
			App::ACTION().subscribe(
				Action::QueuedAction(
					_Action( std::unique_ptr<_interface>( new _wrapper<EventType>( std::move( p_event ) ) ) )
				)
			);
		}
	};

} // namespace VTX::App::Threading

#endif
