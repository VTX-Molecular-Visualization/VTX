#ifndef __VTX_UI_QT_DOCK_WIDGET_CONSOLE__
#define __VTX_UI_QT_DOCK_WIDGET_CONSOLE__

#include <QDockWidget>

namespace VTX::UI::QT::DockWidget
{

	class Console : public QDockWidget
	{
	  public:
		Console( QWidget * p_parent ) : QDockWidget( "Console", p_parent )
		{
			setAllowedAreas( Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea );

			// Set widget.
			setWidget( new QWidget( this ) );
		}

		virtual ~Console() {}

	  private:
	};

} // namespace VTX::UI::QT::DockWidget

#endif
