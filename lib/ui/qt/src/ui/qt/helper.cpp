#include "ui/qt/helper.hpp"

namespace VTX::UI::QT::Helper
{

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
