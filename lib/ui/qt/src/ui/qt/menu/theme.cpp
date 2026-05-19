#include "ui/qt/menu/theme.hpp"
#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/widget/main_window.hpp"
#include <QActionGroup>
#include <app/services.hpp>
#include <string>
#include <util/event_hub.hpp>
#include <util/types.hpp>

namespace VTX::UI::QT::Menu
{
	namespace
	{
		ActionRegistry::ActionParams themeParams( const Style::E_THEME p_theme )
		{
			return ActionRegistry::ActionParams { { std::string( Action::Theme::PARAM_THEME ),
													static_cast<int>( toUnderlying( p_theme ) ) } };
		}
	} // namespace

	Theme::Theme( QWidget * p_parent ) : BaseWidget( p_parent )
	{
		setTitle( "Theme" );

		_actionGroup = new QActionGroup( this );
		_actionGroup->setExclusive( true );

		_addThemeAction( "System", Style::E_THEME::SYSTEM );
		_addThemeAction( "Light", Style::E_THEME::LIGHT );
		_addThemeAction( "Dark", Style::E_THEME::DARK );
		addSeparator();
		UI_ACTIONS().addTo( *this, Action::Theme::RESET_LAYOUT );

		_syncTheme();
		connect( this, &QMenu::aboutToShow, this, &Theme::_syncTheme );
		App::HUB().connect<Events::ThemeChanged, &Theme::_onThemeChanged>( this );
	}

	void Theme::_addThemeAction( const QString & p_label, const Style::E_THEME p_theme )
	{
		QAction * const action = QMenu::addAction( p_label );
		action->setCheckable( true );
		_actionGroup->addAction( action );
		_themeActions[ static_cast<std::size_t>( toUnderlying( p_theme ) ) ] = action;

		connect(
			action,
			&QAction::triggered,
			this,
			[ p_theme ]() { UI_ACTIONS().trigger( Action::Theme::SET, themeParams( p_theme ) ); }
		);
	}

	void Theme::_syncTheme()
	{
		const Style::E_THEME currentTheme = STYLE().getCurrentTheme();
		for ( std::size_t i = 0; i < _themeActions.size(); i++ )
		{
			if ( QAction * const action = _themeActions[ i ] )
			{
				action->setChecked( i == static_cast<std::size_t>( toUnderlying( currentTheme ) ) );
			}
		}
	}

	void Theme::_onThemeChanged( const Events::ThemeChanged & ) { _syncTheme(); }

} // namespace VTX::UI::QT::Menu
