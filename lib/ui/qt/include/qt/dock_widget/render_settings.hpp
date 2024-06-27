#ifndef __VTX_UI_QT_DOCK_WIDGET_RENDER_SETTINGS__
#define __VTX_UI_QT_DOCK_WIDGET_RENDER_SETTINGS__

#include "qt/base_widget.hpp"
#include <QDockWidget>

namespace VTX::UI::QT::DockWidget
{

	class RenderSettings : public BaseWidget<RenderSettings, QDockWidget>
	{
	  public:
		RenderSettings( QWidget * p_parent ) : BaseWidget<RenderSettings, QDockWidget>( "Render settings", p_parent )
		{
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		}

		virtual ~RenderSettings() {}
	};

} // namespace VTX::UI::QT::DockWidget

#endif
