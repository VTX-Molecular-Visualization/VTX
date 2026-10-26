#ifndef __VTX_UI_QT_HELPER__
#define __VTX_UI_QT_HELPER__

#include <QColor>
#include <QLocale>
#include <QObject>
#include <QString>
#include <QtGlobal>
#include <concepts>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <util/color/rgba.hpp>
#include <utility>

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

	/**
	 * @brief QObject factory, returning a non-owning pointer.
	 */
	template<typename T, typename... Args>
		requires std::derived_from<T, QObject>
	[[nodiscard]] T * makeQObject( Args &&... p_args )
	{
		auto object = std::make_unique<T>( std::forward<Args>( p_args )... );

		if ( object->parent() == nullptr )
		{
			throw std::logic_error( "makeQObject: object has no parent" );
		}

		// Qt parent now owns the object.
		// NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
		return object.release();
	}

} // namespace VTX::UI::QT::Helper

#endif
