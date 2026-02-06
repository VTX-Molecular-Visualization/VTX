#include "ui/qt/style/codepoint_icon_engine.hpp"
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QRawFont>
#include <QScreen>

namespace VTX::UI::QT::Style
{

	void CodepointIconEngine::paint(
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

	QPixmap CodepointIconEngine::pixmap( const QSize & p_size, QIcon::Mode p_mode, QIcon::State p_state )
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

} // namespace VTX::UI::QT::Style
