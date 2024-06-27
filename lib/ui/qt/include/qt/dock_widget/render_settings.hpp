#ifndef __VTX_UI_QT_DOCK_WIDGET_RENDER_SETTINGS__
#define __VTX_UI_QT_DOCK_WIDGET_RENDER_SETTINGS__

#include <QDockWidget>

namespace VTX::UI::QT::DockWidget
{

	class RenderSettings : public QDockWidget
	{
	  public:
		RenderSettings( QWidget * p_parent ) : QDockWidget( "Render settings", p_parent )
		{
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		}

		virtual ~RenderSettings() {}
	};

} // namespace VTX::UI::QT::DockWidget

#endif
