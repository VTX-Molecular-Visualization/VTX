#include "ui/qt/menu/theme.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/widget/main_window.hpp"
#include <QActionGroup>

namespace VTX::UI::QT::Menu
{
	Theme::Theme( QWidget * p_parent ) : BaseWidget( p_parent )
	{
		setTitle( "Theme" );

		addAction<Action::Theme::System>();
		addAction<Action::Theme::Light>();
		addAction<Action::Theme::Dark>();
		addSeparator();
		addAction<Action::Theme::ResetLayout>();
	}

} // namespace VTX::UI::QT::Menu
