#include "integer_field_slider_widget.hpp"
#include "style.hpp"
#include "util/math.hpp"
#include "util/ui.hpp"
#include <QHBoxLayout>
#include <string>

namespace VTX::UI::Widget::CustomWidget
{
	IntegerFieldSliderWidget::IntegerFieldSliderWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
	{
		_min = INT16_MIN;
		_max = INT16_MAX;
	};

	void IntegerFieldSliderWidget::_setupUi( const QString & p_name )
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

	void IntegerFieldSliderWidget::_setupSlots()
	{
		connect( _slider,
				 QOverload<const int>::of( &QSlider::valueChanged ),
				 this,
				 &IntegerFieldSliderWidget::_onInternalValueChanged );

		connect( _textField, &QLineEdit::editingFinished, this, &IntegerFieldSliderWidget::_onTextFieldEdited );
	}

	void IntegerFieldSliderWidget::_onTextFieldEdited()
	{
		// Protect data erasment when unselect field with multiple data
		if ( hasDifferentData() && _textField->text() == Style::DIFFERENT_MULTIPLE_DATA_STRING )
			return;

		const int newValue = _textField->text().toInt();
		if ( newValue != _value )
		{
			setValue( newValue );
			emit onValueChange( _value );
		}

		const QString previousText = _textField->text();
		const QString newText	   = getDisplayedText( newValue );

		if ( previousText != newText )
		{
			_refresh();
		}
	}
	void IntegerFieldSliderWidget::_onInternalValueChanged( const int p_newValue )
	{
		if ( p_newValue != _value )
		{
			setValue( p_newValue );
			emit onValueChange( _value );
		}
	}

	void IntegerFieldSliderWidget::_refresh()
	{
		const bool oldBlockState = blockSignals( true );

		_slider->blockSignals( true );
		_slider->setValue( _value );
		_slider->blockSignals( false );

		const QString newText = getDisplayedText( _value );

		if ( newText != _textField->text() )
			_textField->setText( newText );

		blockSignals( oldBlockState );
	}

	void IntegerFieldSliderWidget::localize() {};

	QString IntegerFieldSliderWidget::getDisplayedText( const int p_value ) const
	{
		return QString::fromStdString( std::to_string( p_value ) );
	}

	void IntegerFieldSliderWidget::setValue( const int p_value )
	{
		if ( _value != p_value )
		{
			_value = p_value;
			_refresh();
		}
	};
	void IntegerFieldSliderWidget::setMin( const int p_min )
	{
		_min = p_min;
		_max = _min > _max ? _min : _max;

		_slider->setMinimum( _min );

		_value = _value < _min ? _min : _value;
		_refresh();
	};
	void IntegerFieldSliderWidget::setMax( const int p_max )
	{
		_max = p_max;
		_min = _min > _max ? _max : _min;

		_slider->setMaximum( _max );
		_value = _value > _max ? _max : _value;

		_refresh();
	};
	void IntegerFieldSliderWidget::setMinMax( const int p_min, const int p_max )
	{
		_min = p_min;
		_max = p_max;

		_slider->setMinimum( _min );
		_slider->setMaximum( _max );

		_value = Util::clamp( _value, p_min, p_max );
		_refresh();
	};
	void IntegerFieldSliderWidget::setSingleStep( const int p_step ) { _slider->setSingleStep( p_step ); }
	void IntegerFieldSliderWidget::setPageStep( const int p_step ) { _slider->setPageStep( p_step ); }

	void IntegerFieldSliderWidget::setEnabled( const bool p_enable )
	{
		QWidget::setEnabled( p_enable );
		_slider->setEnabled( p_enable );
		_textField->setEnabled( p_enable );
	}

	void IntegerFieldSliderWidget::resetState()
	{
		TMultiDataFieldEquatable::resetState();
		_refresh();
	}
	void IntegerFieldSliderWidget::_setSingleValue( const int & p_value )
	{
		const bool oldBlockState = blockSignals( true );
		setValue( p_value );
		blockSignals( oldBlockState );
	}

	void IntegerFieldSliderWidget::_displayDifferentsDataFeedback()
	{
		_textField->setText( Style::DIFFERENT_MULTIPLE_DATA_STRING );
	}

} // namespace VTX::UI::Widget::CustomWidget
