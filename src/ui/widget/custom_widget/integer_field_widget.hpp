#ifndef __VTX_UI_WIDGET_CUSTOM_INTEGER_FIELD__
#define __VTX_UI_WIDGET_CUSTOM_INTEGER_FIELD__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/multi_data_field.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QLineEdit>
#include <QSlider>
#include <QValidator>
#include <QWidget>

namespace VTX::UI::Widget::CustomWidget
{
	class IntegerFieldWidget : public BaseManualWidget<QWidget>, public UI::TMultiDataFieldEquatable<const int>
	{
		VTX_WIDGET
		Q_OBJECT

	  public:
		~IntegerFieldWidget() = default;

		const int getValue() const { return _value; };
		void	  setValue( const int p_value );

		void localize() override;
		void setMinMax( const int p_min, const int p_max );
		void setSingleStep( const int p_step );
		void setPageStep( const int p_step );

		void setEnabled( const bool p_enable );

		void resetState() override;

	  signals:
		void onValueChange( const int p_value );

	  protected:
		IntegerFieldWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void _refresh();

		void		_displayDifferentsDataFeedback();
		const int & _getValue() const { return _value; }
		void		_setSingleValue( const int & p_value );

		void _onTextFieldEdited();
		void _onInternalValueChanged( const int p_newValue );

	  private:
		QSlider *	_slider	   = nullptr;
		QLineEdit * _textField = nullptr;

		QIntValidator * _textFieldValidator = nullptr;

		int _value = 0;
		int _min;
		int _max;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
