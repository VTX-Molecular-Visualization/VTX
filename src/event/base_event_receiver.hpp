#ifndef __VTX_BASE_EVENT_RECEIVER__
#define __VTX_BASE_EVENT_RECEIVER__

#ifdef _MSC_VER
#pragma once
#endif

#include "event.hpp"

namespace VTX
{
	namespace Event
	{
		template<typename T>
		class BaseEventReceiver
		{
		  public:
			virtual void receiveEvent( const T & p_event ) {};

		  protected:
			virtual void _registerEvent( const VTX_EVENT & )   = 0;
			virtual void _unregisterEvent( const VTX_EVENT & ) = 0;
		};
	} // namespace Event
} // namespace VTX
#endif
