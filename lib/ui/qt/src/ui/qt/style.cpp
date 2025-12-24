#include "ui/qt/style.hpp"
#include "ui/qt/application.hpp"
#include "ui/qt/resources.hpp"
#include <QFile>
#include <QFontDatabase>
#include <QIcon>
#include <QStyle>
#include <QWidget>
#include <util/logger.hpp>

namespace
{
	using namespace VTX::UI::QT;

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

namespace VTX::UI::QT
{

	void Style::load( const std::vector<App::Tool::BaseTool *> & p_tools )
	{
		using namespace Resources;

		// Set Desktop style.
		Q_APP()->setStyle( "Fusion" );

		// Load main stylesheet.
		QFile stylesheetFile( FILE_STYLESHEET.data() );
		if ( stylesheetFile.open( QFile::ReadOnly ) )
		{
			QString stylesheet = stylesheetFile.readAll();

			// Load os-specific stylesheet.
#if _WIN32
			QFile stylesheetOSFile( FILE_STYLESHEET_WINDOWS.data() );
#elif __linux__
			QFile stylesheetOSFile( FILE_STYLESHEET_LINUX.data() );
#elif __APPLE__
			QFile stylesheetOSFile( FILE_STYLESHEET_MACOS.data() );
#else
			QFile stylesheetOSFile();
			assert( true );
#endif

			if ( stylesheetOSFile.open( QFile::ReadOnly ) )
			{
				stylesheet += '\n' + stylesheetOSFile.readAll();
			}

			for ( const App::Tool::BaseTool * const tool : p_tools )
			{
				if ( tool->getStyle().has_value() )
				{
					stylesheet += '\n' + tool->getStyle().value();
				}
			}

			// Set stylesheet to app.
			Q_APP()->setStyleSheet( stylesheet );
		}

		// Load icons font (broken until 6.9).
		QFontDatabase::addApplicationFont( FONT_MATERIAL_SYMBOLS.data() );
		QIcon::setThemeName( "Material Symbols Outlined" );

		// List all available fonts.
		const QStringList fontList = QFontDatabase::families();
		for ( const QString & fontName : fontList )
		{
			VTX_INFO( "Available font: {}", fontName.toStdString() );
		}

		// Check material symbols font availability.
		if ( !fontList.contains( "Material Symbols Outlined" ) )
		{
			VTX_WARNING( "Material Symbols Outlined font not found. Icons may not be displayed correctly." );
		}

		// Save system palette.
		_themePalettes[ E_THEME::SYSTEM ] = Q_APP()->palette();
		_themePalettes[ E_THEME::LIGHT ]  = _makeLightPalette();
		_themePalettes[ E_THEME::DARK ]	  = _makeDarkPalette();
	}

	void Style::setTheme( const E_THEME p_theme )
	{
		if ( p_theme == _currentTheme )
		{
			return;
		}

		Q_APP()->setPalette( _themePalettes[ p_theme ] );
		for ( QWidget * w : Q_APP()->allWidgets() )
		{
			w->setPalette( _themePalettes[ p_theme ] );
			w->update();
		}
		_currentTheme = p_theme;
	}

} // namespace VTX::UI::QT
