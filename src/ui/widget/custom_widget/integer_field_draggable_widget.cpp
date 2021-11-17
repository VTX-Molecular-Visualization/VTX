#include "integer_field_draggable_widget.hpp"
#include "style.hpp"
#include "util/math.hpp"
#include <QHBoxLayout>
#include <string>

namespace VTX::UI::Widget::CustomWidget
{
	IntegerFieldDraggableWidget::IntegerFieldDraggableWidget( QWidget * p_parent ) :
		BaseManualWidget( p_parent ), TMultiDataFieldEquatable()
	{
		_min = INT_MIN;
		_max = INT_MAX;
	};

	void IntegerFieldDraggableWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		QHBoxLayout * const mainLayout = new QHBoxLayout( this );
		mainLayout->setSpacing( Style::DATA_GRID_HORIZONTAL_SPACE );
		mainLayout->setContentsMargins( 0, 0, 0, 0 );

		_label = new QLabel( this );
		_label->setMouseTracking( true );
		_label->setContentsMargins( 0, 0, 0, 0 );

		_textField = new QLineEdit( this );
		_textField->setContentsMargins( 0, 0, 0, 0 );
		_textField->setInputMethodHints( Qt::InputMethodHint::ImhDigitsOnly );

		mainLayout->addWidget( _label, 1 );
		mainLayout->addWidget( _textField, 10 );

		setMouseTracking( true );
	}

	void IntegerFieldDraggableWidget::_setupSlots()
	{
		connect( _textField, &QLineEdit::editingFinished, this, &IntegerFieldDraggableWidget::_onTextFieldEdited );
	}

	void IntegerFieldDraggableWidget::paintEvent( QPaintEvent * event )
	{
		QWidget::paintEvent( event );

		// TODO find a better place to catch when the window loose focus
		if ( !isActiveWindow() && _hasTarget )
		{
			_hasTarget = false;
			unsetCursor();
		}
	}
	void IntegerFieldDraggableWidget::mousePressEvent( QMouseEvent * p_event )
	{
		const QPoint globalPos = p_event->globalPos();
		_hasTarget			   = _canDragAtPos( globalPos );

		if ( _hasTarget )
		{
			p_event->accept();
			_mousePressPos = globalPos;
			setCursor( Qt::CursorShape::BlankCursor );
		}

		BaseManualWidget::mousePressEvent( p_event );
	}

	void IntegerFieldDraggableWidget::mouseMoveEvent( QMouseEvent * p_event )
	{
		if ( !_hasTarget )
		{
			const bool isHoverDragArea = _canDragAtPos( p_event->globalPos() );

			if ( isHoverDragArea )
				setCursor( Qt::CursorShape::SizeHorCursor );
			else
				unsetCursor();
		}
		else
		{
			const QPoint currentPos = p_event->globalPos();
			const int	 delta		= currentPos.x() - _mousePressPos.x();

			const double valueDelta = delta * double( _dragValueFactor );

			// freeze cursor
			cursor().setPos( _mousePressPos );

			_onValueDragged( valueDelta );

			p_event->accept();
		}

		BaseManualWidget::mouseMoveEvent( p_event );
	}

	void IntegerFieldDraggableWidget::mouseReleaseEvent( QMouseEvent * p_event )
	{
		if ( _hasTarget )
		{
			_hasTarget = false;

			const bool isHoverDragArea = _canDragAtPos( p_event->globalPos() );

			if ( isHoverDragArea )
				setCursor( Qt::CursorShape::SizeHorCursor );
			else
				unsetCursor();
		}

		BaseManualWidget::mouseReleaseEvent( p_event );
	}

	void IntegerFieldDraggableWidget::_onTextFieldEdited()
	{
		const int newValue = _textField->text().toInt();

		if ( newValue != _value )
		{
			setValue( newValue );
			emit onValueChange( _value );
		}

		if ( newValue == 0.f ) // If value == 0, textfield can contain non valid number chain => de a refresh to
							   // force valid display of 0.
		{
			_refresh();
		}
	}

	void IntegerFieldDraggableWidget::_onInternalValueChanged( const int p_value )
	{
		if ( p_value != _value )
		{
			setValue( p_value );
			emit onValueChange( _value );
		}
	}
	void IntegerFieldDraggableWidget::_onValueDragged( const int p_delta )
	{
		if ( p_delta != 0 )
		{
			const int newValue = _value + p_delta;

			setValue( newValue );

			if ( _separateChangeAndDrag )
				emit onValueDragged( p_delta );
			else
				emit onValueChange( newValue );
		}
	}

	void IntegerFieldDraggableWidget::_refresh()
	{
		_textField->setText( QString::fromStdString( std::to_string( _value ) ) );
	}

	void IntegerFieldDraggableWidget::localize() {};

	void IntegerFieldDraggableWidget::setValue( const int p_value )
	{
		const int clampedValue = Util::Math::clamp( p_value, _min, _max );
		if ( _value != clampedValue )
		{
			_value = clampedValue;
			_refresh();
		}
	};

	void IntegerFieldDraggableWidget::setMin( const int p_min )
	{
		_min = p_min;

		_value = _value < _min ? _min : _value;
		_refresh();
	}
	void IntegerFieldDraggableWidget::setMax( const int p_max )
	{
		_max = p_max;

		_value = _value > _max ? _max : _value;
		_refresh();
	}
	void IntegerFieldDraggableWidget::setMinMax( const int p_min, const int p_max )
	{
		_min = p_min;
		_max = p_max;

		_value = Util::Math::clamp( _value, p_min, p_max );
		_refresh();
	};
	void IntegerFieldDraggableWidget::setLabel( const QString & p_label ) { _label->setText( p_label ); }
	void IntegerFieldDraggableWidget::setDragValueFactor( const float p_factor ) { _dragValueFactor = p_factor; };
	void IntegerFieldDraggableWidget::setEnabled( const bool p_enable )
	{
		QWidget::setEnabled( p_enable );
		_label->setEnabled( p_enable );
		_textField->setEnabled( p_enable );
	}
	void IntegerFieldDraggableWidget::separateChangeAndDrag( const bool p_separate )
	{
		_separateChangeAndDrag = p_separate;
	}

	bool IntegerFieldDraggableWidget::_canDragAtPos( const QPoint & p_globalPos ) const
	{
		return _label->rect().contains( _label->mapFromGlobal( p_globalPos ) );
	}

	void IntegerFieldDraggableWidget::resetState()
	{
		TMultiDataFieldEquatable::resetState();
		_refresh();
	}

	void IntegerFieldDraggableWidget::_displayDifferentsDataFeedback()
	{
		_textField->blockSignals( true );
		_textField->setText( "-" );
		_textField->blockSignals( false );
	}

} // namespace VTX::UI::Widget::CustomWidget
