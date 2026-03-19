#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/resources.hpp"
#include "ui/qt/services.hpp"
#include <QIcon>
#include <util/enum.hpp>
#include <util/logger.hpp>

namespace
{
	using namespace VTX::UI::QT::Style;

	QPalette _makeLightPalette()
	{
		QPalette p;

		p.setColor( QPalette::Window, QColor( 240, 240, 240 ) );
		p.setColor( QPalette::WindowText, Qt::black );
		p.setColor( QPalette::Base, Qt::white );
		p.setColor( QPalette::AlternateBase, QColor( 225, 225, 225 ) );
		p.setColor( QPalette::ToolTipBase, Qt::white );
		p.setColor( QPalette::ToolTipText, Qt::black );
		p.setColor( QPalette::Text, Qt::black );
		p.setColor( QPalette::Button, QColor( 240, 240, 240 ) );
		p.setColor( QPalette::ButtonText, Qt::black );
		p.setColor( QPalette::BrightText, Qt::red );

		p.setColor( QPalette::Highlight, QColor( 76, 163, 224 ) );
		p.setColor( QPalette::HighlightedText, Qt::white );

		p.setColor( QPalette::Disabled, QPalette::Text, QColor( 150, 150, 150 ) );
		p.setColor( QPalette::Disabled, QPalette::ButtonText, QColor( 150, 150, 150 ) );

		return p;
	}

	QPalette _makeDarkPalette()
	{
		QPalette p;

		p.setColor( QPalette::Window, QColor( 53, 53, 53 ) );
		p.setColor( QPalette::WindowText, Qt::white );
		p.setColor( QPalette::Base, QColor( 42, 42, 42 ) );
		p.setColor( QPalette::AlternateBase, QColor( 66, 66, 66 ) );
		p.setColor( QPalette::ToolTipBase, Qt::white );
		p.setColor( QPalette::ToolTipText, Qt::white );
		p.setColor( QPalette::Text, Qt::white );
		p.setColor( QPalette::Button, QColor( 53, 53, 53 ) );
		p.setColor( QPalette::ButtonText, Qt::white );
		p.setColor( QPalette::BrightText, Qt::red );

		p.setColor( QPalette::Highlight, QColor( 90, 122, 190 ) );
		p.setColor( QPalette::HighlightedText, Qt::white );

		p.setColor( QPalette::Disabled, QPalette::Text, QColor( 127, 127, 127 ) );
		p.setColor( QPalette::Disabled, QPalette::ButtonText, QColor( 127, 127, 127 ) );

		return p;
	}
} // namespace

namespace VTX::UI::QT::Style
{

	StyleManager::StyleManager()
	{
		using namespace Resources;

		// Set Desktop style.
		Q_APP()->setStyle( "Fusion" );

		QFontDatabase::addApplicationFont( FONT_MATERIAL_SYMBOLS.data() );
		QFontDatabase::addApplicationFont( FONT_INTER.data() );
		QFontDatabase::addApplicationFont( FONT_OPEN_DYSLEXIC.data() );

		// Linux only?
		// QIcon::setThemeName( "Material Symbols Outlined" );

		// Save system palette.
		_themePalettes[ toUnderlying( E_THEME::SYSTEM ) ] = Q_APP()->palette();
		_themePalettes[ toUnderlying( E_THEME::LIGHT ) ]  = _makeLightPalette();
		_themePalettes[ toUnderlying( E_THEME::DARK ) ]	  = _makeDarkPalette();

		// Load theme from settings.
		try
		{
			QString		  themeName = SETTINGS().value( SETTING_KEY_THEME, "SYSTEM" ).toString();
			const E_THEME theme		= Util::Enum::enumCast<E_THEME>( themeName.toStdString() );
			QString		  fontName	= SETTINGS().value( SETTING_KEY_FONT, DEFAULT_FONT_FAMILY ).toString();
			setFontFamily( fontName );
			setTheme( theme );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Failed to restore style from settings: {}", p_e.what() );
			setFontFamily( DEFAULT_FONT_FAMILY );
			setTheme( DEFAULT_THEME );
		}
	}

	StyleManager::~StyleManager()
	{
		const QString themeName = Util::Enum::enumName( _currentTheme ).data();
		SETTINGS().setValue( SETTING_KEY_THEME, themeName );
		SETTINGS().setValue( SETTING_KEY_FONT, getCurrentFontFamily() );
	}

	void StyleManager::setTheme( const E_THEME p_theme )
	{
		auto & pal = _themePalettes[ toUnderlying( p_theme ) ];

		// Ensure inactive highlight color is the same as active.
		pal.setColor( QPalette::Inactive, QPalette::Highlight, pal.color( QPalette::Active, QPalette::Highlight ) );

		Q_APP()->setPalette( pal );
		_currentTheme = p_theme;
	}

	void StyleManager::setFontFamily( const QString & p_fontName )
	{
		QFont appFont( p_fontName, DEFAULT_FONT_SIZE );
		Q_APP()->setFont( appFont );
	}

	QIcon StyleManager::iconFromCodepoint( const Codepoint p_codepoint )
	{
		return QIcon( new CodepointIconEngine( p_codepoint ) );
	}

} // namespace VTX::UI::QT::Style
