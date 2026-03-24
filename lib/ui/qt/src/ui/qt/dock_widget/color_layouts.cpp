#include "ui/qt/dock_widget/color_layouts.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/widget/library/color_layout.hpp"
#include <util/factories.hpp>

namespace VTX::UI::QT::DockWidget
{

	ColorLayouts::ColorLayouts( QWidget * p_parent ) : BaseDockWidget( p_parent, "Colors" )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		setWindowIcon( STYLE().iconFromCodepoint( Style::Icons::COLOR_LAYOUT ) );

		auto * const colorLayoutWidget = new Widget::Library::ColorLayout( this );
		_layout->addWidget( colorLayoutWidget );

		_layout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	}
} // namespace VTX::UI::QT::DockWidget
