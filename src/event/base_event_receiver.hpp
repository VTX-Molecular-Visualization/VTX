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

			virtual void handleEvent( const T & p_event, void * const p_arg = nullptr ) = 0;
		};
	} // namespace Generic
} // namespace VTX
#endif
