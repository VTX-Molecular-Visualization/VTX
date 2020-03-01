#ifndef __VTX_BASE_EVENT_RECEIVER__
#define __VTX_BASE_EVENT_RECEIVER__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace Event
	{
		template<typename T>
		class BaseEventReceiver
		{
		  public:
			virtual void receiveEvent( T * const p_event ) {};

		  protected:
			virtual void _registerEvents()	 = 0;
			virtual void _unregisterEvents() = 0;
		};
	} // namespace Event
} // namespace VTX
#endif
