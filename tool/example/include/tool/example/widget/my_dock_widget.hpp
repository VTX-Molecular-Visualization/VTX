#ifndef VTX_TOOL_EXAMPLE_WIDGET_MY_DOCK_WIDGET_HPP
#define VTX_TOOL_EXAMPLE_WIDGET_MY_DOCK_WIDGET_HPP

#include <QDockWidget>
#include <ui/qt/dock_widget/base_dock_widget.hpp>

namespace VTX::Tool::Example::Widget
{

	class MyDockWidget : public UI::QT::DockWidget::BaseDockWidget<MyDockWidget>
	{
	  public:
		MyDockWidget( QWidget * const p_parent ) : BaseDockWidget( p_parent, "My Dock Panel" )
		{ setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea ); }

		virtual ~MyDockWidget() {}
	};

} // namespace VTX::Tool::Example::Widget

#endif
