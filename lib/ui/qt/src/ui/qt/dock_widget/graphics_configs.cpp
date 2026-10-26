#include "ui/qt/dock_widget/graphics_configs.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/widget/library/graphics_config_widget.hpp"
#include <memory>
#include <util/factories.hpp>

namespace VTX::UI::QT::DockWidget
{

	GraphicsConfigs::GraphicsConfigs( QWidget * const p_parent ) : BaseDockWidget( p_parent, "Graphics" )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		setWindowIcon( STYLE().iconFromCodepoint( Style::Icons::GRAPHICS_CONFIG ) );

		auto * const graphicsConfigWidget = Helper::makeQObject<Widget::Library::GraphicsConfigWidget>( this );
		_layout->addWidget( graphicsConfigWidget );
		_layout->addSpacerItem(
			std::make_unique<QSpacerItem>( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ).release()
		);
	}

} // namespace VTX::UI::QT::DockWidget
