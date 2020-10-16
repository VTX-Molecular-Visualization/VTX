#ifndef __VTX_VIEW_UI_LINKED_WIDGET_INTERFACE__
#define __VTX_VIEW_UI_LINKED_WIDGET_INTERFACE__

#ifdef _MSC_VER
#pragma once
#endif

#include <QWidget>

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class LinkedWidgetInterface
				{
				  public:
					virtual QWidget * getLinkedWidget() = 0;
				};
			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX
#endif
