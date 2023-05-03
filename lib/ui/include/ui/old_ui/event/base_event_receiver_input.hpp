#ifndef __VTX_BASE_EVENT_RECEIVER_INPUT__
#define __VTX_BASE_EVENT_RECEIVER_INPUT__

#include <app/core/event/base_event_receiver.hpp>
#include <app/id.hpp>

namespace VTX
{
	namespace UI::Event
	{
		class EventManager;
	}

	namespace Event
	{
		template<typename T>
		class BaseEventReceiverInput : public VTX::App::Core::Event::BaseEventReceiver<T>
		{
			friend ::VTX::UI::Event::EventManager;

		  public:
			virtual ~BaseEventReceiverInput() = default;

			inline virtual App::VTX_ID getTargetWidget() = 0;
			inline virtual bool readEventFromInput( const App::VTX_ID & p_id ) { return p_id == getTargetWidget(); };

		  protected:
			virtual void receiveEvent( const T & p_event ) override
			{
				VTX::App::Core::Event::BaseEventReceiver<T>::receiveEvent( p_event );
			}
		};
	} // namespace Event
} // namespace VTX
#endif
