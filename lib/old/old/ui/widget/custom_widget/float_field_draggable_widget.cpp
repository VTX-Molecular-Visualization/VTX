#include "float_field_draggable_widget.hpp"
#include "style.hpp"
#include "util/math.hpp"
#include "util/string.hpp"
#include <util/constants.hpp>
#include <QHBoxLayout>
#include <cmath>
#include <string>

namespace VTX::UI::Widget::CustomWidget
{
	FloatFieldDraggableWidget::FloatFieldDraggableWidget( QWidget * p_parent ) :
		BaseManualWidget( p_parent ), TMultiDataFieldEquatable()
	{
		_min = FLOAT_MIN;
		_max = FLOAT_MAX;
	};

	void FloatFieldDraggableWidget::_setupUi( const QString & p_name )
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

		mainLayout->addWidget( _label, 1 );
		mainLayout->addWidget( _textField, 10 );

		_label->installEventFilter( this );

		setMouseTracking( true );
	}

	void FloatFieldDraggableWidget::_setupSlots()
	{
		connect( _textField, &QLineEdit::editingFinished, this, &FloatFieldDraggableWidget::_onTextFieldEdited );
	}

	bool FloatFieldDraggableWidget::eventFilter( QObject * p_obj, QEvent * p_event )
	{
		if ( p_obj == _label )
		{
			if ( p_event->type() == QEvent::MouseMove )
			{
				labelMouseMoveEvent( static_cast<QMouseEvent *>( p_event ) );
			}
			else if ( p_event->type() == QEvent::MouseButtonPress )
			{
				labelMousePressEvent( static_cast<QMouseEvent *>( p_event ) );
			}
			else if ( p_event->type() == QEvent::MouseButtonRelease )
			{
				labelMouseReleaseEvent( static_cast<QMouseEvent *>( p_event ) );
			}
		}

		return QObject::eventFilter( p_obj, p_event );
	}

	void FloatFieldDraggableWidget::paintEvent( QPaintEvent * event )
	{
		QWidget::paintEvent( event );

		// TODO find a better place to catch when the window loose focus
		if ( !isActiveWindow() && _hasTarget )
		{
			_hasTarget = false;
			_label->unsetCursor();
		}
	}
	void FloatFieldDraggableWidget::labelMousePressEvent( QMouseEvent * p_event )
	{
		const QPoint globalPos = p_event->globalPos();
		_hasTarget			   = _canDragAtPos( globalPos );

		if ( _hasTarget )
		{
			p_event->accept();
			_mousePressPos = globalPos;
			_label->setCursor( Qt::CursorShape::BlankCursor );
		}
	}

	void FloatFieldDraggableWidget::labelMouseMoveEvent( QMouseEvent * p_event )
	{
		if ( !_hasTarget )
		{
			const bool isHoverDragArea = _canDragAtPos( p_event->globalPos() );

			if ( isHoverDragArea )
				_label->setCursor( Qt::CursorShape::SizeHorCursor );
			else
				_label->unsetCursor();
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
	}

	void FloatFieldDraggableWidget::labelMouseReleaseEvent( QMouseEvent * p_event )
	{
		if ( _hasTarget )
		{
			_hasTarget = false;

			const bool isHoverDragArea = _canDragAtPos( p_event->globalPos() );

			if ( isHoverDragArea )
				_label->setCursor( Qt::CursorShape::SizeHorCursor );
			else
				_label->unsetCursor();
		}
	}

	void FloatFieldDraggableWidget::_onTextFieldEdited()
	{
		// Protect data erasment when unselect field with multiple data
		if ( hasDifferentData() && _textField->text() == Style::DIFFERENT_MULTIPLE_DATA_STRING )
			return;

		const float newValue = _textField->text().toFloat();

		if ( newValue != _value || hasDifferentData() )
		{
			setValue( newValue );
			emit onValueChange( _value );
		}

		const QString previousText = _textField->text();
		const QString newText	   = getDisplayedText( newValue );

		if ( previousText != newText ) // Force text update to normalize display (for example 1 == 1.00, but the display
									   // must be 1.00 and not 1)
		{
			_refresh();
		}
	}

	void FloatFieldDraggableWidget::_onInternalValueChanged( const float p_value )
	{
		if ( abs( p_value - _value ) > _epsilon )
		{
			setValue( p_value );
			emit onValueChange( _value );
		}
	}
	void FloatFieldDraggableWidget::_onValueDragged( const float p_delta )
	{
		if ( abs( p_delta ) > _epsilon )
		{
			const float newValue = _value + p_delta;

			setValue( newValue );

			if ( _separateChangeAndDrag )
				emit onValueDragged( p_delta );
			else
				emit onValueChange( newValue );
		}
	}

	void FloatFieldDraggableWidget::_refresh()
	{
		const QString newText = getDisplayedText( _value );

		if ( newText != _textField->text() )
			_textField->setText( newText );
	}

	void FloatFieldDraggableWidget::localize() {};

	void FloatFieldDraggableWidget::setValue( const float p_value )
	{
		const float clampedValue = Util::Math::clamp( p_value, _min, _max );
		if ( _value != clampedValue )
		{
			_value = clampedValue;
			_refresh();
		}
	};

	QString FloatFieldDraggableWidget::getDisplayedText( const float p_value ) const
	{
		return QString::fromStdString( Util::String::floatToStr( p_value, _nbDecimals ) );
	}

	void FloatFieldDraggableWidget::setNbDecimals( const int p_nbDecimals )
	{
		_nbDecimals = p_nbDecimals;
		_epsilon	= float( 1 / std::pow( _nbDecimals, 10 ) );
		_refresh();
	}

	void FloatFieldDraggableWidget::setMin( const float p_min )
	{
		_min = p_min;

		_value = _value < _min ? _min : _value;
		_refresh();
	}
	void FloatFieldDraggableWidget::setMax( const float p_max )
	{
		_max = p_max;

		_value = _value > _max ? _max : _value;
		_refresh();
	}
	void FloatFieldDraggableWidget::setMinMax( const float p_min, const float p_max )
	{
		_min = p_min;
		_max = p_max;

		_value = Util::Math::clamp( _value, p_min, p_max );
		_refresh();
	};
	void FloatFieldDraggableWidget::setLabel( const QString & p_label ) { _label->setText( p_label ); }
	void FloatFieldDraggableWidget::setDragValueFactor( const float p_factor ) { _dragValueFactor = p_factor; };
	void FloatFieldDraggableWidget::setEnabled( const bool p_enable )
	{
		QWidget::setEnabled( p_enable );
		_label->setEnabled( p_enable );
		_textField->setEnabled( p_enable );
	}
	void FloatFieldDraggableWidget::separateChangeAndDrag( const bool p_separate )
	{
		_separateChangeAndDrag = p_separate;
	}

	bool FloatFieldDraggableWidget::_canDragAtPos( const QPoint & p_globalPos ) const
	{
		return _label->rect().contains( _label->mapFromGlobal( p_globalPos ) );
	}

	void FloatFieldDraggableWidget::resetState()
	{
		TMultiDataFieldEquatable::resetState();
		_refresh();
	}

	void FloatFieldDraggableWidget::_displayDifferentsDataFeedback()
	{
		_textField->blockSignals( true );
		_textField->setText( Style::DIFFERENT_MULTIPLE_DATA_STRING );
		_textField->blockSignals( false );
	}

} // namespace VTX::UI::Widget::CustomWidget
