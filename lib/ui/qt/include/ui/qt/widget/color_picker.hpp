#ifndef __VTX_UI_QT_WIDGET_COLOR_PICKER__
#define __VTX_UI_QT_WIDGET_COLOR_PICKER__

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

		inline const QColor & getColor() const { return _color; }
		inline QColor		  getColor() { return _color; }
		void				  setColor( const QColor & );

		Util::Callback<QColor> onColorChanged;

	  private:
		QColor _color;
	};
} // namespace VTX::UI::QT::Widget
#endif
