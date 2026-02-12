#ifndef __VTX_UI_QT_HELPER__
#define __VTX_UI_QT_HELPER__

#include <QColor>
#include <util/color/rgba.hpp>

namespace VTX::UI::QT::Helper
{
	/**
	 * @brief Conversion between QColor and Rgba
	 */
	VTX::Util::Color::Rgba fromQColor( const QColor & );
	QColor				   toQColor( const VTX::Util::Color::Rgba & );

} // namespace VTX::UI::QT::Helper

#endif
