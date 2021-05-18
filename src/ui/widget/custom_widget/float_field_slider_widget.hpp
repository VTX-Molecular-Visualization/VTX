#ifndef __VTX_UI_WIDGET_CUSTOM_FLOAT_FIELD_SLIDER__
#define __VTX_UI_WIDGET_CUSTOM_FLOAT_FIELD_SLIDER__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/multi_data_field.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QLineEdit>
#include <QSlider>
#include <QWidget>

namespace VTX::UI::Widget::CustomWidget
{
	class FloatFieldSliderWidget : public BaseManualWidget<QWidget>, public UI::TMultiDataFieldEquatable<const float>
	{
		VTX_WIDGET
		Q_OBJECT

	  public:
		~FloatFieldSliderWidget() = default;

		const float getValue() const { return _value; };
		void		setValue( const float p_value );

		void localize() override;
		void setNbDecimals( const int p_nbDecimals );
		void setMin( const float p_min );
		void setMax( const float p_max );
		void setMinMax( const float p_min, const float p_max );
		void setEnabled( const bool p_enable );

		// MultiDataField Implementation //////////////////////////////
		void resetState() override;
		//////////////////////////////////////////////////////////////

	  signals:
		void onValueChange( const float p_value );

	  protected:
		FloatFieldSliderWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void _refresh();

		// MultiDataField Implementation //////////////////////////////
		void		  _displayDifferentsDataFeedback();
		const float & _getValue() const { return _value; }
		void		  _setSingleValue( const float & p_value );
		//////////////////////////////////////////////////////////////

		void _onTextFieldEdited();
		void _onInternalValueChanged( const int p_newValue );

		void _emitOnValueChangeSignal();

	  private:
		QSlider *	_slider	   = nullptr;
		QLineEdit * _textField = nullptr;

		int _nbDecimals = 2;

		float _value = 0;
		float _min;
		float _max;

		int _blockSignalsCounter = 0;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
