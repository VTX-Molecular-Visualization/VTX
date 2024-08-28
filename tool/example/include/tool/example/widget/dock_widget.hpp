#ifndef __VTX_TOOL_EXAMPLE_WIDGET_DOCK_WIDGET__
#define __VTX_TOOL_EXAMPLE_WIDGET_DOCK_WIDGET__

#include <QDockWidget>
#include <qt/base_widget.hpp>

namespace VTX::Tool::Example::Widget
{

	class DockWidget : public UI::QT::BaseWidget<DockWidget, QDockWidget>
	{
	  public:
		DockWidget( QWidget * p_parent ) : BaseWidget<DockWidget, QDockWidget>( "My dock panel", p_parent )
		{
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		}

		virtual ~DockWidget() {}
	};

} // namespace VTX::Tool::Example::Widget

#endif
