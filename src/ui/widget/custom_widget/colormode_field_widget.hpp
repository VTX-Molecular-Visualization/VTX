#ifndef __VTX_UI_WIDGET_COLORMODE_FIELD_WIDGET__
#define __VTX_UI_WIDGET_COLORMODE_FIELD_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "color/rgb.hpp"
#include "color_field_button.hpp"
#include "generic/base_colorable.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QBoxLayout>
#include <QComboBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>

namespace VTX::UI::Widget::CustomWidget
{
	class ColorModeFieldWidget : public BaseManualWidget<QWidget>
	{
		Q_OBJECT
		VTX_WIDGET

	  public:
		~ColorModeFieldWidget() {};
		void localize() override;

		const Generic::COLOR_MODE & getColorMode() const { return _colorMode; };
		void						setColorMode( const Generic::COLOR_MODE p_colorMode );

		const Color::Rgb & getColor() { return _color; };
		void			   setColor( const Color::Rgb & p_color );

	  signals:
		void colorModeChanged( const Generic::COLOR_MODE & p_colorMode );
		void colorChanged( const Color::Rgb & p_color );

	  protected:
		ColorModeFieldWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refresh();

		void _colorModeChange( int index );
		void _applyColor( const Color::Rgb & p_color );
		void _openColorSettings();

	  private:
		Generic::COLOR_MODE _colorMode;
		Color::Rgb			_color;

		QHBoxLayout * _layout;

		ColorFieldButton * _colorSetButton;
		QPushButton *	   _openColorSettingsButton;
		QComboBox *		   _colorModeComboBox;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
