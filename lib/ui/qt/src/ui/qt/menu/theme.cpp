#include "ui/qt/menu/theme.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/widget/main_window.hpp"

namespace VTX::UI::QT::Menu
{
	Theme::Theme( QWidget * p_parent ) : BaseWidget( "Theme", p_parent )
	{
		addAction( MAIN_WINDOW().getAction<Action::Theme::System>() );
		addSeparator();
		addAction( MAIN_WINDOW().getAction<Action::Theme::Light>() );
		addAction( MAIN_WINDOW().getAction<Action::Theme::Dark>() );
		addSeparator();
		auto * const aReset = MAIN_WINDOW().getAction<Action::Theme::ResetLayout>();
		addAction( aReset );
		connect( aReset, &QAction::triggered, this, &Theme::_resetLayout );
	}

	void Theme::_resetLayout()
	{
		VTX_TRACE( "Ask layout reset" );
		MAIN_WINDOW().resetLayout();
		VTX_TRACE( "Layout reseted" );
	}

} // namespace VTX::UI::QT::Menu
