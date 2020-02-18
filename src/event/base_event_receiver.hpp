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
			virtual void registerEvents()												 = 0;
			virtual void unregisterEvents()												 = 0;
			virtual void receiveEvent( const T & p_event, void * const p_arg = nullptr ) = 0;
		};
	} // namespace Event
} // namespace VTX
#endif
