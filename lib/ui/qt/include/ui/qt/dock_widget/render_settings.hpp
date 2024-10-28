#ifndef __VTX_UI_QT_DOCK_WIDGET_RENDER_SETTINGS__
#define __VTX_UI_QT_DOCK_WIDGET_RENDER_SETTINGS__

#include "ui/qt/core/base_dock_widget.hpp"
#include <QDockWidget>

namespace VTX::App::Component::Representation
{
	class RenderSettings;
}

namespace VTX::UI::QT::DockWidget
{
	class RenderSettings : public Core::BaseDockWidget<RenderSettings>
	{
	  public:
		RenderSettings( QWidget * p_parent );

	  private:
		void _createGroupBoxShading( App::Component::Representation::RenderSettings * const );
		void _createGroupBoxSSAO( App::Component::Representation::RenderSettings * const );
		void _createGroupBoxOutline( App::Component::Representation::RenderSettings * const );
		void _createGroupBoxFog( App::Component::Representation::RenderSettings * const );
		void _createGroupBoxSelection( App::Component::Representation::RenderSettings * const );
	};

} // namespace VTX::UI::QT::DockWidget

#endif
