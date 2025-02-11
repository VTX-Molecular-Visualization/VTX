#ifndef __VTX_UI_QT_WIDGET_STATUS_BAR__
#define __VTX_UI_QT_WIDGET_STATUS_BAR__

#include "ui/qt/core/base_widget.hpp"
#include <QLabel>
#include <QPointer>
#include <QStatusBar>
#include <QTimer>

namespace VTX::UI::QT::Widget
{

	class StatusBar : public Core::BaseWidget<StatusBar, QStatusBar>
	{
	  public:
		StatusBar( QWidget * p_parent );
	};

} // namespace VTX::UI::QT::Widget

#endif
