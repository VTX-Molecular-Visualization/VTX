#ifndef __VTX_UI_QT_HELPER__
#define __VTX_UI_QT_HELPER__

#include <QColor>
#include <QLocale>
#include <QString>
#include <QtGlobal>
#include <type_traits>
#include <util/color/rgba.hpp>

namespace VTX::UI::QT::Helper
{
	/**
	 * @brief Center a widget in a given rectangle.
	 */
	void centerWidget( QWidget &, const QRect & );

	/**
	 * @brief Conversion between QColor and Rgba
	 */
	VTX::Util::Color::Rgba fromQColor( const QColor & );
	QColor				   toQColor( const VTX::Util::Color::Rgba & );

	/**
	 * @brief Format number with current locale.
	 */
	template<typename T>
	QString formatNumber( const T p_value, const char p_format = 'g', const int p_precision = 6 )
	{
		using Value = std::remove_cv_t<T>;

		static_assert(
			std::is_arithmetic_v<Value> && not std::is_same_v<Value, bool>, "formatNumber expects a numeric value"
		);

		if constexpr ( std::is_floating_point_v<Value> )
		{
			return QLocale().toString( static_cast<double>( p_value ), p_format, p_precision );
		}
		else if constexpr ( std::is_signed_v<Value> )
		{
			return QLocale().toString( static_cast<qlonglong>( p_value ) );
		}
		else
		{
			return QLocale().toString( static_cast<qulonglong>( p_value ) );
		}
	}

} // namespace VTX::UI::QT::Helper

#endif
