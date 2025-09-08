#include "ui/qt/dock_widget/representations.hpp"
#include "ui/qt/widget/library/representation.hpp"
#include <util/factories.hpp>

namespace VTX::UI::QT::DockWidget
{

	Representations::Representations( QWidget * p_parent ) :
		BaseDockWidget<Representations>( "Representations", p_parent )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

		auto * const representationWidget = Util::Factories::newInit<Widget::Library::Representation>( this );
		_layout->addWidget( representationWidget );
		_layout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	}

} // namespace VTX::UI::QT::DockWidget
