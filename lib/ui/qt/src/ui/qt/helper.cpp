#include "ui/qt/helper.hpp"
#include <QWidget>

namespace VTX::UI::QT::Helper
{
	void centerWidget( QWidget & p_widget, const QRect & p_rect )
	{
		const int x = ( p_rect.width() - p_widget.width() ) / 2;
		const int y = ( p_rect.height() - p_widget.height() ) / 2;
		p_widget.move( x, y );
	}

	QColor toQColor( const Util::Color::Rgba & p_color )
	{
		// return QColor( p_color.x * 255, p_color.y * 255, p_color.z * 255, p_color.w * 255 );
		return QColor( QString::fromStdString( p_color.toHexaString() ) );
	}

	Util::Color::Rgba fromQColor( const QColor & p_color )
	{
		return Util::Color::Rgba( p_color.redF(), p_color.greenF(), p_color.blueF(), p_color.alphaF() );
	}
} // namespace VTX::UI::QT::Helper
