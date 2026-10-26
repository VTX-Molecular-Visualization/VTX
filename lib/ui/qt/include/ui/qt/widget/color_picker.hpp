#ifndef VTX_UI_QT_WIDGET_COLOR_PICKER_HPP
#define VTX_UI_QT_WIDGET_COLOR_PICKER_HPP

#include <QColor>
#include <QPushButton>
#include <util/callback.hpp>
#include <util/color/rgba.hpp>

namespace VTX::UI::QT::Widget
{
	class ColorPicker : public QPushButton
	{
	  public:
		ColorPicker( QWidget * );
		ColorPicker( const QColor &, QWidget * );

		const QColor & getColor() const { return _color; }

		QColor getColor() { return _color; }

		void setColor( const QColor & );

		Util::Callback<QColor> onColorChanged;

	  private:
		QColor _color;
	};
} // namespace VTX::UI::QT::Widget
#endif
