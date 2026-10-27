#include "ui/qt/dock_widget/representations.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/widget/library/representation.hpp"
#include <memory>

namespace VTX::UI::QT::DockWidget
{

	Representations::Representations( QWidget * const p_parent ) : BaseDockWidget( p_parent, "Representations" )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		setWindowIcon( STYLE().iconFromCodepoint( Style::Icons::REPRESENTATION ) );

		auto * const representationWidget = Helper::makeQObject<Widget::Library::Representation>( this );
		_layout->addWidget( representationWidget );
		_layout->addSpacerItem(
			std::make_unique<QSpacerItem>( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ).release() );
	}

} // namespace VTX::UI::QT::DockWidget
