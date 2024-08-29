#ifndef __VTX_UI_QT_DOCK_WIDGET_COLORS__
#define __VTX_UI_QT_DOCK_WIDGET_COLORS__

#include "ui/qt/base_widget.hpp"
#include <QDockWidget>

namespace VTX::UI::QT::DockWidget
{

	class Colors : public BaseWidget<Colors, QDockWidget>
	{
	  public:
		Colors( QWidget * p_parent ) : BaseWidget<Colors, QDockWidget>( "Colors", p_parent )
		{
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		}

		virtual ~Colors() {}
	};

} // namespace VTX::UI::QT::DockWidget

#endif
