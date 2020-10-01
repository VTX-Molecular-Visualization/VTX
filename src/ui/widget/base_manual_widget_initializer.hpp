#ifndef __VTX_UI_WIDGET_BASE_MANUAL_WIDGET_INITIALIZER__
#define __VTX_UI_WIDGET_BASE_MANUAL_WIDGET_INITIALIZER__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class WidgetInitializer
			{
			  public:
				WidgetInitializer() {};

				virtual void setupUi()	  = 0;
				virtual void setupSlots() = 0;
				virtual void localize()	  = 0;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX

#endif
