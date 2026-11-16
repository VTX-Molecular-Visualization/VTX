#ifndef VTX_UI_QT_WIDGET_COLOR_PICKER_HPP
#define VTX_UI_QT_WIDGET_COLOR_PICKER_HPP

#include <QColor>
#include <QPushButton>
#include <util/color/rgba.hpp>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Custom color picker.
	 */
	class ColorPicker : public QPushButton
	{
		Q_OBJECT

	  public:
		/**
		 * @brief Constructors.
		 */
		ColorPicker( QWidget * );
		ColorPicker( const QColor &, QWidget * );

		/**
		 * @brief Accessors.
		 */
		const QColor & getColor() const { return _color; }

		QColor getColor() { return _color; }

		void setColor( const QColor & );

	  signals:
		/**
		 * @brief Color changed.
		 */
		void colorChanged( const QColor & );

	  private:
		/**
		 * @brief Actual color.
		 */
		QColor _color;
	};
} // namespace VTX::UI::QT::Widget
#endif
