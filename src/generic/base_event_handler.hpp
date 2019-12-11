#ifndef __VTX_BASE_EVENT_HANDLER__
#define __VTX_BASE_EVENT_HANDLER__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace Generic
	{
		template<typename T>
		class BaseEventHandler
		{
		  public:
			virtual void handleEvent( const T & ) = 0;
		};
	} // namespace Generic
} // namespace VTX
#endif
