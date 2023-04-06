#ifndef __VTX_BASE_EVENT_RECEIVER_VTX__
#define __VTX_BASE_EVENT_RECEIVER_VTX__

#include "base_event_receiver.hpp"
#include "event.hpp"
#include <vector>

namespace VTX
{
	namespace Event
	{
		class BaseEventReceiverVTX : public BaseEventReceiver<VTXEvent>
		{
		  public:
			BaseEventReceiverVTX() = default;
			virtual ~BaseEventReceiverVTX();

		  protected:
			void _registerEvent( const VTX_EVENT & );
			void _unregisterEvent( const VTX_EVENT & );

		  private:
			std::vector<VTX_EVENT> _events = std::vector<VTX_EVENT>();
		};
	} // namespace Event
} // namespace VTX
#endif
