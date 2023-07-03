#ifndef __VTX_BASE_EVENT_RECEIVER_INPUT__
#define __VTX_BASE_EVENT_RECEIVER_INPUT__

#include <app/old/core/event/base_event_receiver.hpp>
#include <app/old/id.hpp>

namespace VTX
{
	namespace UI::Event
	{
		class EventManager;
	}

	namespace Event
	{
		template<typename T>
		class BaseEventReceiverInput : public VTX::App::Old::Core::Event::BaseEventReceiver<T>
		{
			friend ::VTX::UI::Event::EventManager;

		  public:
			virtual ~BaseEventReceiverInput() = default;

			inline virtual App::Old::VTX_ID getTargetWidget() = 0;
			inline virtual bool readEventFromInput( const App::Old::VTX_ID & p_id ) { return p_id == getTargetWidget(); };

		  protected:
			virtual void receiveEvent( const T & p_event ) override
			{
				VTX::App::Old::Core::Event::BaseEventReceiver<T>::receiveEvent( p_event );
			}
		};
	} // namespace Event
} // namespace VTX
#endif
