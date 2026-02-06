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

		auto * system = addAction<Action::Theme::System>();
		auto * light  = addAction<Action::Theme::Light>();
		auto * dark	  = addAction<Action::Theme::Dark>();
		addSeparator();
		connect( addAction<Action::Theme::ResetLayout>(), &QAction::triggered, this, &Theme::_resetLayout );
	}

	void Theme::_resetLayout() { MAIN_WINDOW().resetLayout(); }

} // namespace VTX::UI::QT::Menu
