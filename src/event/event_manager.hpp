#ifndef __VTX_EVENT_MANAGER__
#define __VTX_EVENT_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_event_receiver_keyboard.hpp"
#include "base_event_receiver_mouse.hpp"
#include "base_event_receiver_vtx.hpp"
#include "base_event_receiver_wheel.hpp"
#include "event/event.hpp"
#include "generic/base_lockable.hpp"
#include "generic/base_updatable.hpp"
#include "id.hpp"
#include "model/base_model.hpp"
#include "view/base_view.hpp"
#include <map>
#include <queue>
#include <set>

#define DELAY_EVENTS

namespace VTX
{
	namespace Event
	{
		class EventManager : public Generic::BaseUpdatable, public Generic::BaseLocakble
		{
		  public:
			using SetBaseEventReceiverVTXPtr			 = std::set<BaseEventReceiverVTX *>;
			using MapStringVectorBaseEventReceiverVTXPtr = std::map<Event::VTX_EVENT, SetBaseEventReceiverVTXPtr>;

			using QueueVTXEventPtr = std::queue<VTXEvent *>;

			void registerEventReceiverVTX( const Event::VTX_EVENT &, BaseEventReceiverVTX * const );
			void unregisterEventReceiverVTX( const Event::VTX_EVENT &, BaseEventReceiverVTX * const );
			void registerEventReceiverKeyboard( BaseEventReceiverKeyboard * const );
			void unregisterEventReceiverKeyboard( BaseEventReceiverKeyboard * const );
			void registerEventReceiverMouse( BaseEventReceiverMouse * const );
			void unregisterEventReceiverMouse( BaseEventReceiverMouse * const );
			void registerEventReceiverWheel( BaseEventReceiverWheel * const );
			void unregisterEventReceiverWheel( BaseEventReceiverWheel * const );

			void fireEventVTX( VTXEvent * const );
			void fireEventKeyboard( const QKeyEvent & );
			void fireEventMouse( const QMouseEvent & );
			void fireEventWheel( const QWheelEvent & );

			virtual void update( const double & p_deltaTime ) override;

		  private:
			// VTX receivers mapped on event ID.
			MapStringVectorBaseEventReceiverVTXPtr _receiversVTX = MapStringVectorBaseEventReceiverVTXPtr();
			// Current VTX event queue.
			QueueVTXEventPtr _eventQueue = QueueVTXEventPtr();

			// Input events.
			std::set<BaseEventReceiverKeyboard *> _receiversKeyboard = std::set<BaseEventReceiverKeyboard *>();
			std::set<BaseEventReceiverMouse *>	  _receiversMouse	 = std::set<BaseEventReceiverMouse *>();
			std::set<BaseEventReceiverWheel *>	  _receiversWheel	 = std::set<BaseEventReceiverWheel *>();

			// void _handlerWindowEvent( const SDL_WindowEvent & );
			void _flushVTXEvent( VTXEvent * const );
		};
	} // namespace Event

} // namespace VTX
#endif
