#ifndef __VTX_UI_EVENT_MANAGER__
#define __VTX_UI_EVENT_MANAGER__

#include "base_event_receiver_keyboard.hpp"
#include "base_event_receiver_mouse.hpp"
#include "base_event_receiver_wheel.hpp"
#include <app/core/event/vtx_event.hpp>
#include <app/old_app/generic/base_lockable.hpp>
#include <app/old_app/generic/base_updatable.hpp>
#include <app/old_app/id.hpp>
#include <map>
#include <queue>
#include <set>

// #define DELAY_EVENTS_QT

namespace VTX::UI::Event
{
	class BaseEventFirererInput;

	class EventManager final : public Generic::BaseUpdatable, public Generic::BaseLockable
	{
		friend BaseEventFirererInput;

	  public:
		inline static EventManager & get()
		{
			static EventManager instance;
			return instance;
		}

		void registerEventReceiverKeyboard( VTX::Event::BaseEventReceiverKeyboard * const );
		void unregisterEventReceiverKeyboard( VTX::Event::BaseEventReceiverKeyboard * const );
		void registerEventReceiverMouse( VTX::Event::BaseEventReceiverMouse * const );
		void unregisterEventReceiverMouse( VTX::Event::BaseEventReceiverMouse * const );
		void registerEventReceiverWheel( VTX::Event::BaseEventReceiverWheel * const );
		void unregisterEventReceiverWheel( VTX::Event::BaseEventReceiverWheel * const );

		virtual void update( const float & p_deltaTime ) override;

		void clearKeyboardInputEvents() const;
		void clearKeyboardInputEvent( const Qt::Key & p_key ) const;

		void freezeEvent( const bool p_freeze );

	  private:
		void fireEventKeyboard( QKeyEvent * const, const ID::VTX_ID & );
		void fireEventMouse( QMouseEvent * const, const ID::VTX_ID & );
		void fireEventWheel( QWheelEvent * const, const ID::VTX_ID & );

	  private:
		// Input events.
		std::set<VTX::Event::BaseEventReceiverKeyboard *> _receiversKeyboard
			= std::set<VTX::Event::BaseEventReceiverKeyboard *>();
		std::set<VTX::Event::BaseEventReceiverMouse *> _receiversMouse
			= std::set<VTX::Event::BaseEventReceiverMouse *>();
		std::set<VTX::Event::BaseEventReceiverWheel *> _receiversWheel
			= std::set<VTX::Event::BaseEventReceiverWheel *>();

		// Event queues.
		std::queue<QKeyEvent *>	  _eventQueueKeyboard = std::queue<QKeyEvent *>();
		std::queue<QMouseEvent *> _eventQueueMouse	  = std::queue<QMouseEvent *>();
		std::queue<QWheelEvent *> _eventQueueWheel	  = std::queue<QWheelEvent *>();

		bool _freeze = false;

		EventManager()									 = default;
		EventManager( const EventManager & )			 = delete;
		EventManager & operator=( const EventManager & ) = delete;
		~EventManager();

		void _flushEventKeyboard( QKeyEvent * const, const ID::VTX_ID & );
		void _flushEventMouse( QMouseEvent * const, const ID::VTX_ID & );
		void _flushEventWheel( QWheelEvent * const, const ID::VTX_ID & );
	};
} // namespace VTX::UI::Event
#endif
