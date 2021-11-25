#ifndef __VTX_EVENT_MANAGER__
#define __VTX_EVENT_MANAGER__

#include "base_event_receiver_keyboard.hpp"
#include "base_event_receiver_mouse.hpp"
#include "base_event_receiver_vtx.hpp"
#include "base_event_receiver_wheel.hpp"
#include "event/event.hpp"
#include "generic/base_lockable.hpp"
#include "generic/base_updatable.hpp"
#include "id.hpp"
#include <map>
#include <queue>
#include <set>

//#define DELAY_EVENTS
//#define DELAY_EVENTS_QT

namespace VTX
{
	namespace Event
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

			void registerEventReceiverVTX( const Event::VTX_EVENT &, BaseEventReceiverVTX * const );
			void unregisterEventReceiverVTX( const Event::VTX_EVENT &, BaseEventReceiverVTX * const );
			void registerEventReceiverKeyboard( BaseEventReceiverKeyboard * const );
			void unregisterEventReceiverKeyboard( BaseEventReceiverKeyboard * const );
			void registerEventReceiverMouse( BaseEventReceiverMouse * const );
			void unregisterEventReceiverMouse( BaseEventReceiverMouse * const );
			void registerEventReceiverWheel( BaseEventReceiverWheel * const );
			void unregisterEventReceiverWheel( BaseEventReceiverWheel * const );

			virtual void update( const float & p_deltaTime ) override;

			void fireEventVTX( VTXEvent * const );

			void freezeEvent( const bool p_freeze );
			void clearKeyboardInputEvents() const;
			void clearKeyboardInputEvent( const Qt::Key & p_key ) const;

		  private:
			void fireEventKeyboard( QKeyEvent * const, const ID::VTX_ID & );
			void fireEventMouse( QMouseEvent * const, const ID::VTX_ID & );
			void fireEventWheel( QWheelEvent * const, const ID::VTX_ID & );

		  private:
			// Input events.
			std::map<Event::VTX_EVENT, std::set<BaseEventReceiverVTX *>> _receiversVTX
				= std::map<Event::VTX_EVENT, std::set<BaseEventReceiverVTX *>>(); // VTX receivers mapped on event ID.
			std::set<BaseEventReceiverKeyboard *> _receiversKeyboard = std::set<BaseEventReceiverKeyboard *>();
			std::set<BaseEventReceiverMouse *>	  _receiversMouse	 = std::set<BaseEventReceiverMouse *>();
			std::set<BaseEventReceiverWheel *>	  _receiversWheel	 = std::set<BaseEventReceiverWheel *>();

			// Event queues.
			std::queue<VTXEvent *> _eventQueueVTX = std::queue<VTXEvent *>();

			std::queue<QKeyEvent *>	  _eventQueueKeyboard = std::queue<QKeyEvent *>();
			std::queue<QMouseEvent *> _eventQueueMouse	  = std::queue<QMouseEvent *>();
			std::queue<QWheelEvent *> _eventQueueWheel	  = std::queue<QWheelEvent *>();

			bool _freeze = false;

			EventManager()						 = default;
			EventManager( const EventManager & ) = delete;
			EventManager & operator=( const EventManager & ) = delete;
			~EventManager();

			void _flushVTXEvent( VTXEvent * const );
			void _flushEventKeyboard( QKeyEvent * const, const ID::VTX_ID & );
			void _flushEventMouse( QMouseEvent * const, const ID::VTX_ID & );
			void _flushEventWheel( QWheelEvent * const, const ID::VTX_ID & );
		};
	} // namespace Event

	inline void VTX_EVENT( VTX::Event::VTXEvent * const p_event )
	{
		Event::EventManager::get().fireEventVTX( p_event );
	}
} // namespace VTX
#endif
