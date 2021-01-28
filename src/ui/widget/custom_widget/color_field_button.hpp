#ifndef __VTX_UI_WIDGET_CUSTOM_COLOR_FIELD_BUTTON__
#define __VTX_UI_WIDGET_CUSTOM_COLOR_FIELD_BUTTON__

#ifdef _MSC_VER
#pragma once
#endif

#include "color/rgb.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QColor>
#include <QColorDialog>
#include <QImage>
#include <QPaintEvent>
#include <QPushButton>

namespace VTX::UI::Widget::CustomWidget
{
	class ColorFieldButton : public BaseManualWidget<QPushButton>
	{
		VTX_WIDGET
		Q_OBJECT

	  public:
		~ColorFieldButton() = default;

		const Color::Rgb & getColor() const { return _color; };
		void			   setColor( const Color::Rgb & p_color );

		void localize() override;

		void setEnabled( const bool p_enable );

	  signals:
		void onValueChange( const Color::Rgb & p_newColor );

	  protected:
		ColorFieldButton( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void _refresh();

		virtual void paintEvent( QPaintEvent * ) override;

	  private:
		QColorDialog * _colorDialog;
		QImage		   _colorImage;

		Color::Rgb _color;

		void _fillImageWithColor( const Color::Rgb & p_color );

		void	   _onClickButton();
		void	   _onColorChanged( const QColor & p_color );
		Color::Rgb _getRgbColorFromQColor( const QColor & p_color ) const;
		QColor	   _getQColorFromRgbColor( const Color::Rgb & p_color ) const;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
