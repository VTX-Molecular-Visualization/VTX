#ifndef __VTX_TOOL_EXAMPLE_WIDGET_MY_DOCK_WIDGET__
#define __VTX_TOOL_EXAMPLE_WIDGET_MY_DOCK_WIDGET__

#include <QDockWidget>
#include <ui/qt/core/base_widget.hpp>

namespace VTX::Tool::Example::Widget
{

	class MyDockWidget : public UI::QT::Core::BaseWidget<MyDockWidget, QDockWidget>
	{
	  public:
		MyDockWidget( QWidget * p_parent ) : BaseWidget<MyDockWidget, QDockWidget>( "My dock panel", p_parent )
		{
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		}

		virtual ~MyDockWidget() {}
	};

} // namespace VTX::Tool::Example::Widget

#endif
