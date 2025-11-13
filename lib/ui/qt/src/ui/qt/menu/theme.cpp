#include "ui/qt/menu/theme.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style.hpp"
#include "ui/qt/widget/main_window.hpp"
#include <QActionGroup>
#include <util/enum.hpp>

namespace
{
	constexpr std::string_view _SETTING_KEY_THEME = "theme/current";
}

namespace VTX::UI::QT::Menu
{
	Theme::Theme( QWidget * p_parent ) : BaseWidget( "Theme", p_parent )
	{
		auto * system = addAction<Action::Theme::System>();
		addSeparator();
		auto * light = addAction<Action::Theme::Light>();
		auto * dark	 = addAction<Action::Theme::Dark>();
		addSeparator();
		connect( addAction<Action::Theme::ResetLayout>(), &QAction::triggered, this, &Theme::_resetLayout );

		// Settings.
		QString themeName = SETTINGS().value( _SETTING_KEY_THEME, "SYSTEM" ).toString();
		try
		{
			const Style::E_THEME theme = Util::Enum::enumCast<Style::E_THEME>( themeName.toStdString() );

			STYLE().setTheme( theme );

			switch ( theme )
			{
			case Style::E_THEME::SYSTEM: system->setChecked( true ); break;
			case Style::E_THEME::LIGHT: dark->setChecked( true ); break;
			case Style::E_THEME::DARK: dark->setChecked( true ); break;
			default: break;
			}
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Failed to restore theme from settings: {}", p_e.what() );
			STYLE().setTheme( Style::E_THEME::SYSTEM );
			system->setChecked( true );
		}
	}

	Theme::~Theme()
	{
		const Style::E_THEME theme	   = STYLE().getCurrentTheme();
		const QString		 themeName = Util::Enum::enumName( theme ).data();

		SETTINGS().setValue( _SETTING_KEY_THEME, themeName );
	}

	void Theme::_resetLayout() { MAIN_WINDOW().resetLayout(); }

} // namespace VTX::UI::QT::Menu
