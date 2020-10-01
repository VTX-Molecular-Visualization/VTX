#ifndef __VTX_UI_WIDGET_BASE_MANUAL_WIDGET_INITIALIZER__
#define __VTX_UI_WIDGET_BASE_MANUAL_WIDGET_INITIALIZER__

#ifdef _MSC_VER
#pragma once
#endif

#define VTX_MANUAL_WIDGET_DECLARATION friend class WidgetFactory;

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class BaseManualWidgetInitializer
			{
				VTX_MANUAL_WIDGET_DECLARATION

			  public:
				virtual void localize() = 0;

			  protected:
				BaseManualWidgetInitializer() {};

				virtual void setupUi()	  = 0;
				virtual void setupSlots() = 0;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX

#endif
