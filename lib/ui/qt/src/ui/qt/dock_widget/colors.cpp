#include "ui/qt/dock_widget/colors.hpp"
#include <app/application/scene.hpp>

namespace VTX::UI::QT::DockWidget
{

	Colors::Colors( QWidget * p_parent ) : BaseWidget<Colors, QDockWidget>( "Colors", p_parent )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

		App::SCENE().onDefaultColorLayout += [ this ]( const App::Component::Representation::ColorLayout & p_layout )
		{
			//
		};
	}
} // namespace VTX::UI::QT::DockWidget
