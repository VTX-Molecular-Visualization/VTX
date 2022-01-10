#ifndef __VTX_UI_WIDGET_RENDER_BASE_INTEGRATED_WIDGET__
#define __VTX_UI_WIDGET_RENDER_BASE_INTEGRATED_WIDGET__

#include "ui/widget/base_manual_widget.hpp"
#include <QWidget>

namespace VTX::UI::Widget::Render
{
	class BaseIntegratedWidget
	{
	  public:
		virtual void updatePosition() = 0;
	};
} // namespace VTX::UI::Widget::Render
#endif
