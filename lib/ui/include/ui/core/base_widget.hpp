#ifndef __VTX_UI_CORE_BASE_WIDGET__
#define __VTX_UI_CORE_BASE_WIDGET__

#include <app/event/base_event_receiver_vtx.hpp>

namespace VTX::UI::Core
{
	class BaseWidget : public VTX::Event::BaseEventReceiverVTX
	{
	  public:
		BaseWidget() = default;

		virtual void setup() = 0;
	};
} // namespace VTX::UI::Core
#endif
