#ifndef __VTX_BASE_EVENT_RECEIVER_INPUT__
#define __VTX_BASE_EVENT_RECEIVER_INPUT__

#include "base_event_receiver.hpp"
#include "id.hpp"

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
		};
	} // namespace Event
} // namespace VTX
#endif
