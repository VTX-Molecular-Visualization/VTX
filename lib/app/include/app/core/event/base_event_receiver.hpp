#ifndef __VTX_APP_CORE_EVENT_BASE_EVENT_RECEIVER__
#define __VTX_APP_CORE_EVENT_BASE_EVENT_RECEIVER__

namespace VTX::Core::Event
{
	class EventManager;

	template<typename T>
	class BaseEventReceiver
	{
		friend EventManager;

	  protected:
		virtual void receiveEvent( const T & p_event ) {}
	};
} // namespace VTX::Core::Event
#endif
