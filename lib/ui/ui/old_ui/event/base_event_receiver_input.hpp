#ifndef __VTX_BASE_EVENT_RECEIVER_INPUT__
#define __VTX_BASE_EVENT_RECEIVER_INPUT__

#include <old/event/base_event_receiver.hpp>
#include <old/id.hpp>

namespace VTX
{
	namespace Event
	{
		template<typename T>
		class BaseEventReceiverInput : public BaseEventReceiver<T>
		{
		  public:
			virtual ~BaseEventReceiverInput() = default;

			inline virtual ID::VTX_ID getTargetWidget() = 0;
			inline virtual bool readEventFromInput( const ID::VTX_ID & p_id ) { return p_id == getTargetWidget(); };
		};
	} // namespace Event
} // namespace VTX
#endif
