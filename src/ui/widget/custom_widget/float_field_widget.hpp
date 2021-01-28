#ifndef __VTX_UI_WIDGET_CUSTOM_FLOAT_FIELD__
#define __VTX_UI_WIDGET_CUSTOM_FLOAT_FIELD__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/widget/base_manual_widget.hpp"
#include <QDoubleValidator>
#include <QLineEdit>
#include <QSlider>
#include <QWidget>

namespace VTX::UI::Widget::CustomWidget
{
	class FloatFieldWidget : public BaseManualWidget<QWidget>
	{
		VTX_WIDGET
		Q_OBJECT

	  public:
		~FloatFieldWidget() = default;

		const float getValue() const { return _value; };
		void		setValue( const float p_value );

		void localize() override;
		void setNbDecimals( const int p_nbDecimals );
		void setMinMax( const float p_min, const float p_max );
		void setEnabled( const bool p_enable );

	  signals:
		void onValueChange( const float p_value );

	  protected:
		FloatFieldWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void _refresh();

		void _onTextFieldEdited();
		void _onInternalValueChanged( const int p_newValue );

	  private:
		QSlider *	_slider	   = nullptr;
		QLineEdit * _textField = nullptr;

		QDoubleValidator * _textFieldValidator = nullptr;

		int _nbDecimals = 2;

		float _value = 0;
		float _min;
		float _max;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
