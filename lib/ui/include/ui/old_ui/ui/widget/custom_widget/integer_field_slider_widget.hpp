#ifndef __VTX_UI_WIDGET_CUSTOM_INTEGER_FIELD__
#define __VTX_UI_WIDGET_CUSTOM_INTEGER_FIELD__

#include "ui/old_ui/ui/multi_data_field.hpp"
#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include <QLineEdit>
#include <QSlider>
#include <QString>
#include <QValidator>
#include <QWidget>

namespace VTX::UI::Widget::CustomWidget
{
	class IntegerFieldSliderWidget : public BaseManualWidget<QWidget>, public UI::TMultiDataFieldEquatable<const int>
	{
		VTX_WIDGET
		Q_OBJECT

	  public:
		~IntegerFieldSliderWidget() = default;

		const int getValue() const { return _value; };
		void	  setValue( const int p_value );

		void localize() override;
		int	 getMin() { return _min; };
		int	 getMax() { return _max; };
		void setMin( const int p_min );
		void setMax( const int p_max );
		void setMinMax( const int p_min, const int p_max );
		void setSingleStep( const int p_step );
		void setPageStep( const int p_step );

		void setEnabled( const bool p_enable );

		void resetState() override;

	  signals:
		void onValueChange( const int p_value );

	  protected:
		IntegerFieldSliderWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void _refresh();

		QString getDisplayedText( const int p_value ) const;

		void		_displayDifferentsDataFeedback() override;
		const int & _getValue() const override { return _value; }
		void		_setSingleValue( const int & p_value ) override;

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
