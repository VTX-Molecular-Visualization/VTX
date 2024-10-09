#ifndef __VTX_UI_QT_DOCK_WIDGET_RENDER_SETTINGS__
#define __VTX_UI_QT_DOCK_WIDGET_RENDER_SETTINGS__

#include "ui/qt/base_widget.hpp"
#include <QDockWidget>

namespace VTX::UI::QT::DockWidget
{

	class RenderSettings : public BaseWidget<RenderSettings, QDockWidget>
	{
	  public:
		RenderSettings( QWidget * p_parent ) : BaseWidget<RenderSettings, QDockWidget>( "Render settings", p_parent )
		{
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

			auto * widget = new QWidget( this );
			auto * layout = new QVBoxLayout( widget );
			layout->setContentsMargins( 0, 0, 0, 0 );
			layout->setSizeConstraint( QLayout::SetNoConstraint );
			setWidget( widget );
		}
	};

} // namespace VTX::UI::QT::DockWidget

#endif
