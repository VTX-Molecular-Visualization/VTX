#include "ui/qt/dock_widget/representations.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style.hpp"
#include "ui/qt/widget/library/representation.hpp"
#include <util/factories.hpp>

namespace VTX::UI::QT::DockWidget
{

	Representations::Representations( QWidget * p_parent ) : BaseDockWidget( p_parent, "Representations" )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		setWindowIcon( STYLE().iconFromCodepoint( 0xe429 ) );

		auto * const representationWidget = new Widget::Library::Representation( this );
		_layout->addWidget( representationWidget );
		_layout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	}

} // namespace VTX::UI::QT::DockWidget
