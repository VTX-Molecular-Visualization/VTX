#ifndef __VTX_UI_QT_DOCK_WIDGET_RENDER_SETTINGS__
#define __VTX_UI_QT_DOCK_WIDGET_RENDER_SETTINGS__

#include "ui/qt/core/base_dock_widget.hpp"
#include <QDockWidget>

namespace VTX::UI::QT::DockWidget
{

	class RenderSettings : public Core::BaseDockWidget<RenderSettings>
	{
	  public:
		RenderSettings( QWidget * p_parent );
	};

} // namespace VTX::UI::QT::DockWidget

#endif
