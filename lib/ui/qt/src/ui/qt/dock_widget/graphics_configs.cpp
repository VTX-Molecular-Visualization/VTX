#include "ui/qt/dock_widget/graphics_configs.hpp"
#include "ui/qt/widget/library/graphics_config.hpp"
#include <util/factories.hpp>

namespace VTX::UI::QT::DockWidget
{

	GraphicsConfigs::GraphicsConfigs( QWidget * p_parent ) : BaseDockWidget( p_parent )
	{
		setWindowTitle( "Graphics" );
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

		auto * const graphicsConfigWidget = new Widget::Library::GraphicsConfig( this );
		_layout->addWidget( graphicsConfigWidget );
		_layout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	}

} // namespace VTX::UI::QT::DockWidget
