#ifndef __VTX_BASE_EVENT_FIRERER__
#define __VTX_BASE_EVENT_FIRERER__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace Generic
	{
		template<typename T>
		class BaseEventFirerer
		{
		  public:
			virtual ~BaseEventFirerer() = default;

			virtual void fireEvent( const T & p_event, void * const p_arg = nullptr ) = 0;
		};
	} // namespace Generic
} // namespace VTX
#endif
