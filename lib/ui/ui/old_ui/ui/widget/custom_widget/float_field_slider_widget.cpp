#include "float_field_slider_widget.hpp"
#include "old_ui/style.hpp"
#include "old_ui/util/ui.hpp"
#include <QHBoxLayout>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include <util/constants.hpp>
#include <util/math.hpp>
#include <util/string.hpp>

namespace VTX::UI::Widget::CustomWidget
{
	FloatFieldSliderWidget::FloatFieldSliderWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
	{
		_min = FLOAT_MIN;
		_max = FLOAT_MAX;
	};

	void FloatFieldSliderWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		QHBoxLayout * const mainLayout = new QHBoxLayout( this );
		mainLayout->setSpacing( 2 );
		mainLayout->setContentsMargins( 0, 0, 0, 0 );

		_slider = new QSlider( Qt::Orientation::Horizontal, this );
		_slider->setContentsMargins( 0, 0, 0, 0 );
		Util::UI::filterEventOnWidget( _slider, QEvent::Type::Wheel );

		_textField = new QLineEdit( this );
		_textField->setContentsMargins( 0, 0, 0, 0 );

		mainLayout->addWidget( _slider, 10 );
		mainLayout->addWidget( _textField, 1 );
	}

	void FloatFieldSliderWidget::_setupSlots()
	{
		connect( _slider,
				 QOverload<const int>::of( &QSlider::valueChanged ),
				 this,
				 &FloatFieldSliderWidget::_onInternalValueChanged );

		connect( _textField, &QLineEdit::editingFinished, this, &FloatFieldSliderWidget::_onTextFieldEdited );
	}

	void FloatFieldSliderWidget::_onTextFieldEdited()
	{
		// Protect data erasment when unselect field with multiple data
		if ( hasDifferentData() && _textField->text() == Style::DIFFERENT_MULTIPLE_DATA_STRING )
			return;

		const float newValue = _textField->text().toFloat();
		if ( newValue != _value )
		{
			setValue( newValue );
			_emitOnValueChangeSignal();
		}

		const QString previousText = _textField->text();
		const QString newText	   = getDisplayedText( newValue );

		if ( previousText != newText ) // Force text update to normalize display (for example 1 == 1.00, but the display
									   // must be 1.00 and not 1)
		{
			_refresh();
		}
	}
	void FloatFieldSliderWidget::_onInternalValueChanged( const int p_sliderValue )
	{
		const float newValue = _min + ( p_sliderValue / 100.0f ) * ( _max - _min );

		if ( abs( newValue - _value ) > FLT_EPSILON )
		{
			setValue( newValue );
			_emitOnValueChangeSignal();
		}
	}

	void FloatFieldSliderWidget::_refresh()
	{
		const bool oldBlockState = blockSignals( true );

		const int sliderValue = int( std::round( ( ( _value - _min ) / ( _max - _min ) ) * 100 ) );
		_slider->blockSignals( true );
		_slider->setValue( sliderValue );
		_slider->blockSignals( false );

		const QString newText = getDisplayedText( _value );

		if ( newText != _textField->text() )
			_textField->setText( newText );

		blockSignals( oldBlockState );
	}

	void FloatFieldSliderWidget::localize() {};

	void FloatFieldSliderWidget::setValue( const float p_value )
	{
		const float clampedValue = Util::Math::clamp( p_value, _min, _max );
		if ( _value != clampedValue )
		{
			_value = clampedValue;
			_refresh();
		}
	};

	void FloatFieldSliderWidget::setNbDecimals( const int p_nbDecimals ) { _nbDecimals = p_nbDecimals; }

	void FloatFieldSliderWidget::setMin( const float p_min )
	{
		_min = p_min;

		if ( _value < _min )
		{
			_value = _min;
			_emitOnValueChangeSignal();
		}

		_refresh();
	}
	void FloatFieldSliderWidget::setMax( const float p_max )
	{
		_max = p_max;

		if ( _value > _max )
		{
			_value = _max;
			_emitOnValueChangeSignal();
		}

		_refresh();
	}
	void FloatFieldSliderWidget::setMinMax( const float p_min, const float p_max )
	{
		_min = p_min;
		_max = p_max;

		_slider->setMinimum( 0 );
		_slider->setMaximum( 100 );

		_value = Util::Math::clamp( _value, p_min, p_max );
		_refresh();
	};
	void FloatFieldSliderWidget::setEnabled( const bool p_enable )
	{
		QWidget::setEnabled( p_enable );
		_slider->setEnabled( p_enable );
		_textField->setEnabled( p_enable );
	}

	void FloatFieldSliderWidget::_emitOnValueChangeSignal()
	{
		if ( !signalsBlocked() )
			emit onValueChange( _value );
	}

	QString FloatFieldSliderWidget::getDisplayedText( const float p_value ) const
	{
		return QString::fromStdString( Util::String::floatToStr( p_value, _nbDecimals ) );
	}

	void FloatFieldSliderWidget::resetState()
	{
		TMultiDataFieldEquatable::resetState();
		_refresh();
	}

	void FloatFieldSliderWidget::_setSingleValue( const float & p_value )
	{
		const bool oldBlockState = blockSignals( true );
		setValue( p_value );
		blockSignals( oldBlockState );
	}

	void FloatFieldSliderWidget::_displayDifferentsDataFeedback()
	{
		_textField->setText( Style::DIFFERENT_MULTIPLE_DATA_STRING );
	}
} // namespace VTX::UI::Widget::CustomWidget
