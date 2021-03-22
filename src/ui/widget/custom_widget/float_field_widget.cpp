#include "float_field_widget.hpp"
#include "util/math.hpp"
#include <QHBoxLayout>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

namespace VTX::UI::Widget::CustomWidget
{
	FloatFieldWidget::FloatFieldWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
	{
		_min = FLOAT_MIN;
		_max = FLOAT_MAX;
	};

	void FloatFieldWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		QHBoxLayout * const mainLayout = new QHBoxLayout( this );
		mainLayout->setSpacing( 2 );

		_slider				= new QSlider( Qt::Orientation::Horizontal, this );
		_textField			= new QLineEdit( this );
		_textFieldValidator = new QDoubleValidator();
		_textFieldValidator->setRange( _min, _max );

		mainLayout->addWidget( _slider, 10 );
		mainLayout->addWidget( _textField, 1 );
	}

	void FloatFieldWidget::_setupSlots()
	{
		connect( _slider,
				 QOverload<const int>::of( &QSlider::valueChanged ),
				 this,
				 &FloatFieldWidget::_onInternalValueChanged );

		connect( _textField, &QLineEdit::editingFinished, this, &FloatFieldWidget::_onTextFieldEdited );
	}

	void FloatFieldWidget::_onTextFieldEdited()
	{
		const float newValue = _textField->text().toFloat();
		if ( newValue != _value )
		{
			setValue( newValue );
			emit onValueChange( _value );
		}
	}
	void FloatFieldWidget::_onInternalValueChanged( const int p_sliderValue )
	{
		const float newValue = _min + ( p_sliderValue / 100.0f ) * ( _max - _min );

		if ( abs( newValue - _value ) > FLT_EPSILON )
		{
			setValue( newValue );
			emit onValueChange( _value );
		}
	}

	void FloatFieldWidget::_refresh()
	{
		const int sliderValue = int( std::round( ( ( _value - _min ) / ( _max - _min ) ) * 100 ) );
		_slider->setValue( sliderValue );

		std::stringstream strStream;
		strStream << std::fixed << std::setprecision( _nbDecimals ) << _value;
		_textField->setText( QString::fromStdString( strStream.str() ) );
	}

	void FloatFieldWidget::localize() {};

	void FloatFieldWidget::setValue( const float p_value )
	{
		const float clampedValue = Util::Math::clamp( p_value, _min, _max );
		if ( _value != clampedValue )
		{
			_value = clampedValue;
			_refresh();
		}
	};

	void FloatFieldWidget::setNbDecimals( const int p_nbDecimals ) { _nbDecimals = p_nbDecimals; }
	void FloatFieldWidget::setMinMax( const float p_min, const float p_max )
	{
		_min = p_min;
		_max = p_max;

		_textFieldValidator->setRange( _min, _max );
		_slider->setMinimum( 0 );
		_slider->setMaximum( 100 );

		_value = Util::Math::clamp( _value, p_min, p_max );
		_refresh();
	};
	void FloatFieldWidget::setEnabled( const bool p_enable )
	{
		QWidget::setEnabled( p_enable );
		_slider->setEnabled( p_enable );
		_textField->setEnabled( p_enable );
	}

} // namespace VTX::UI::Widget::CustomWidget
