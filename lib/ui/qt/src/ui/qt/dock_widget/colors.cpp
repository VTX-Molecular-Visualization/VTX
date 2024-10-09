#include "ui/qt/dock_widget/colors.hpp"

namespace VTX::UI::QT::DockWidget
{

	Colors::Colors( QWidget * p_parent ) : BaseWidget<Colors, QDockWidget>( "Colors", p_parent )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
	}

} // namespace VTX::UI::QT::DockWidget
