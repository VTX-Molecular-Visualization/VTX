#ifndef __VTX_UI_WIDGET_BASE_MANUAL_WIDGET_INITIALIZER__
#define __VTX_UI_WIDGET_BASE_MANUAL_WIDGET_INITIALIZER__

#ifdef _MSC_VER
#pragma once
#endif

#include <QtCore>
#include "ui/widget_factory.hpp"

namespace VTX
{
	namespace UI
	{
		class WidgetFactory;
	}
}

#define VTX_MANUAL_WIDGET_DECLARATION friend ::VTX::UI::WidgetFactory;

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class BaseManualWidgetInitializer
			{
				friend ::VTX::UI::WidgetFactory;

			  public:
				virtual void localize() = 0;

			  protected:
				BaseManualWidgetInitializer() {};

				virtual void setupUi( const QString & p_name ) = 0;
				virtual void setupSlots()					   = 0;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX

#endif
