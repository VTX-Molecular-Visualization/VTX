#include "integer_field_widget.hpp"
#include "util/math.hpp"
#include <QHBoxLayout>
#include <string>

namespace VTX::UI::Widget::CustomWidget
{
	IntegerFieldWidget::IntegerFieldWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
	{
		_min = INT16_MIN;
		_max = INT16_MAX;
	};

	void IntegerFieldWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		QHBoxLayout * const mainLayout = new QHBoxLayout( this );
		mainLayout->setSpacing( 2 );

		_slider				= new QSlider( Qt::Orientation::Horizontal, this );
		_textField			= new QLineEdit( this );
		_textFieldValidator = new QIntValidator();
		_textFieldValidator->setRange( _min, _max );

		mainLayout->addWidget( _slider, 10 );
		mainLayout->addWidget( _textField, 1 );
	}

	void IntegerFieldWidget::_setupSlots()
	{
		connect( _slider,
				 QOverload<const int>::of( &QSlider::valueChanged ),
				 this,
				 &IntegerFieldWidget::_onInternalValueChanged );

		connect( _textField, &QLineEdit::editingFinished, this, &IntegerFieldWidget::_onTextFieldEdited );
	}

	void IntegerFieldWidget::_onTextFieldEdited()
	{
		const int newValue = _textField->text().toInt();
		if ( newValue != _value )
		{
			setValue( newValue );
			emit onValueChange( _value );
		}
	}
	void IntegerFieldWidget::_onInternalValueChanged( const int p_newValue )
	{
		if ( p_newValue != _value )
		{
			setValue( p_newValue );
			emit onValueChange( _value );
		}
	}

	void IntegerFieldWidget::_refresh()
	{
		_slider->setValue( _value );
		_textField->setText( QString::fromStdString( std::to_string( _value ) ) );
	}

	void IntegerFieldWidget::localize() {};

	void IntegerFieldWidget::setValue( const int p_value )
	{
		if ( _value != p_value )
		{
			_value = p_value;
			_refresh();
		}
	};
	void IntegerFieldWidget::setMinMax( const int p_min, const int p_max )
	{
		_min = p_min;
		_max = p_max;

		_textFieldValidator->setRange( _min, _max );
		_slider->setMinimum( _min );
		_slider->setMaximum( _max );

		_value = Util::Math::clamp( _value, p_min, p_max );
		_refresh();
	};
	void IntegerFieldWidget::setEnabled( const bool p_enable )
	{
		QWidget::setEnabled( p_enable );
		_slider->setEnabled( p_enable );
		_textField->setEnabled( p_enable );
	}

} // namespace VTX::UI::Widget::CustomWidget
