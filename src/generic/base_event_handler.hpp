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
			virtual ~BaseEventHandler() = default;

			virtual void handleEvent( const T &	   p_eventType,
									  const double p_timeDelta,
									  void * const p_arg = nullptr ) {};
		};
	} // namespace Generic
} // namespace VTX
#endif
