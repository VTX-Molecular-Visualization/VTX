#include "ui/qt/style.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/application.hpp"
#include "ui/qt/resources.hpp"
#include "ui/qt/services.hpp"
#include <QPainter>
#include <QPainterPath>
#include <QRawFont>
#include <QScreen>
#include <util/enum.hpp>
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

	Style::Style() {}

	Style::~Style()
	{
		const QString themeName = Util::Enum::enumName( _currentTheme ).data();
		SETTINGS().setValue( SETTING_KEY_THEME, themeName );
		SETTINGS().setValue( SETTING_KEY_FONT, getCurrentFontFamily() );
	}

	void Style::load( const std::vector<App::Tool::BaseTool *> & p_tools )
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
			QFont		  appFont( fontName, 10 );

			setFontFamily( fontName );

			// Trigger action group to update checked action.
			auto * QActionGroup = Application::getAction<Action::Theme::System>()->actionGroup();

			switch ( theme )
			{
			case VTX::UI::QT::E_THEME::SYSTEM: Application::getAction<Action::Theme::System>()->trigger(); break;
			case VTX::UI::QT::E_THEME::LIGHT: Application::getAction<Action::Theme::Light>()->trigger(); break;
			case VTX::UI::QT::E_THEME::DARK: Application::getAction<Action::Theme::Dark>()->trigger(); break;
			case VTX::UI::QT::E_THEME::COUNT:;
			default: break;
			}
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Failed to restore style from settings: {}", p_e.what() );
			setFontFamily( DEFAULT_FONT_FAMILY );
			setTheme( DEFAULT_THEME );
		}
	}

	void Style::setTheme( const E_THEME p_theme )
	{
		auto & pal = _themePalettes[ toUnderlying( p_theme ) ];

		// Ensure inactive highlight color is the same as active.
		pal.setColor( QPalette::Inactive, QPalette::Highlight, pal.color( QPalette::Active, QPalette::Highlight ) );

		Q_APP()->setPalette( pal );
		_currentTheme = p_theme;
	}

	void Style::setFontFamily( const QString & p_fontName )
	{
		QFont appFont( p_fontName, DEFAULT_FONT_SIZE );
		Q_APP()->setFont( appFont );
	}

	QIcon Style::iconFromCodepoint( const int p_codepoint )
	{
		return QIcon( new Style::CodepointIconEngine( p_codepoint ) );
	}

	void Style::CodepointIconEngine::paint(
		QPainter *	  p_painter,
		const QRect & p_rect,
		QIcon::Mode	  p_mode,
		QIcon::State  p_state
	)
	{
		if ( p_painter == nullptr || p_rect.isEmpty() )
			return;

		p_painter->save();
		p_painter->setRenderHint( QPainter::Antialiasing, true );

		const QPalette pal = qApp->palette();

		QColor color = pal.color( QPalette::ButtonText );
		switch ( p_mode )
		{
		case QIcon::Disabled: color.setAlphaF( 0.35 ); break;
		case QIcon::Selected: color = pal.color( QPalette::HighlightedText ); break;
		case QIcon::Active:;
		case QIcon::Normal:
		default: break;
		}

		QFont f( DEFAULT_FONT_FAMILY_ICONS );
		f.setStyleStrategy( QFont::NoFontMerging );
		f.setHintingPreference( QFont::PreferNoHinting );

		const char32_t cp = static_cast<char32_t>( _codepoint );
		const QString  s  = QString::fromUcs4( &cp, 1 );

		const QRawFont		   raw	  = QRawFont::fromFont( f );
		const QVector<quint32> glyphs = raw.glyphIndexesForString( s );
		if ( glyphs.isEmpty() || glyphs[ 0 ] == 0 )
		{
			p_painter->restore();
			return;
		}

		QPainterPath path = raw.pathForGlyph( glyphs[ 0 ] );
		QRectF		 br	  = path.boundingRect();
		if ( br.isEmpty() )
		{
			p_painter->restore();
			return;
		}

		const qreal	 margin = 1.0;
		const QRectF target = QRectF( p_rect ).adjusted( margin, margin, -margin, -margin );

		const qreal sx	 = target.width() / br.width();
		const qreal sy	 = target.height() / br.height();
		const qreal smin = std::min( sx, sy );

		QTransform t;
		t.translate( target.center().x(), target.center().y() );
		t.scale( smin, smin );
		t.translate( -br.center().x(), -br.center().y() );

		path = t.map( path );

		p_painter->setPen( Qt::NoPen );
		p_painter->setBrush( color );
		p_painter->drawPath( path );

		p_painter->restore();
	}

	QPixmap Style::CodepointIconEngine::pixmap( const QSize & p_size, QIcon::Mode p_mode, QIcon::State p_state )
	{
		qreal dpr = 1.0;
		if ( QScreen * s = QGuiApplication::primaryScreen() )
		{
			dpr = s->devicePixelRatio();
		}

		QPixmap pm( QSize( int( std::ceil( p_size.width() * dpr ) ), int( std::ceil( p_size.height() * dpr ) ) ) );
		pm.setDevicePixelRatio( dpr );
		pm.fill( Qt::transparent );

		QPainter painter( &pm );
		paint( &painter, QRect( QPoint( 0, 0 ), p_size ), p_mode, p_state );

		return pm;
	}

} // namespace VTX::UI::QT
