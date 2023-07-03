#ifndef __VTX_APP_OLD_CORE_EVENT_BASE_EVENT_RECEIVER__
#define __VTX_APP_OLD_CORE_EVENT_BASE_EVENT_RECEIVER__

namespace VTX::App::Old
{
	namespace Manager
	{
		class EventManager;
	}

	namespace Core::Event
	{
		template<typename T>
		class BaseEventReceiver
		{
			friend Manager::EventManager;

		  protected:
			virtual void receiveEvent( const T & p_event ) {}
		};
	} // namespace Core::Event

} // namespace VTX::App::Old
#endif
