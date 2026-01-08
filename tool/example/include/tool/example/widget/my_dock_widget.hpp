#ifndef __VTX_TOOL_EXAMPLE_WIDGET_MY_DOCK_WIDGET__
#define __VTX_TOOL_EXAMPLE_WIDGET_MY_DOCK_WIDGET__

#include <QDockWidget>
#include <ui/qt/dock_widget/base_dock_widget.hpp>

namespace VTX::Tool::Example::Widget
{

	class MyDockWidget : public UI::QT::DockWidget::BaseDockWidget<MyDockWidget>
	{
	  public:
		MyDockWidget( QWidget * p_parent ) : BaseDockWidget( p_parent )
		{
			setWindowTitle( "My Dock Panel" );
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		}

		virtual ~MyDockWidget() {}
	};

} // namespace VTX::Tool::Example::Widget

#endif
