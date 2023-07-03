#ifndef __VTX_APP_OLD_CORE_EVENT_BASE_EVENT_RECEIVER_VTX__
#define __VTX_APP_OLD_CORE_EVENT_BASE_EVENT_RECEIVER_VTX__

#include "app/old/core/event/base_event_receiver.hpp"
#include "vtx_event.hpp"
#include <vector>

namespace VTX::App::Old::Core::Event
{
	class BaseEventReceiverVTX : public Core::Event::BaseEventReceiver<VTXEvent>
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
} // namespace VTX::App::Old::Core::Event
#endif
