#include "float_field_draggable_widget.hpp"
#include "util/math.hpp"
#include "style.hpp"
#include <QHBoxLayout>
#include <cmath>
#include <iomanip>
#include <sstream>
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

		setMouseTracking( true );
	}

	void FloatFieldDraggableWidget::_setupSlots()
	{
		connect( _textField, &QLineEdit::editingFinished, this, &FloatFieldDraggableWidget::_onTextFieldEdited );
	}

	void FloatFieldDraggableWidget::paintEvent( QPaintEvent * event )
	{
		QWidget::paintEvent( event );

		// TODO find a better place to catch when the window loose focus
		if ( !isActiveWindow() && _hasTarget )
		{
			_hasTarget = false;
			unsetCursor();
		}
	}
	void FloatFieldDraggableWidget::mousePressEvent( QMouseEvent * p_event )
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

	void FloatFieldDraggableWidget::mouseMoveEvent( QMouseEvent * p_event )
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
			_onInternalValueChanged( _value + valueDelta );

			p_event->accept();
		}

		BaseManualWidget::mouseMoveEvent( p_event );
	}

	void FloatFieldDraggableWidget::mouseReleaseEvent( QMouseEvent * p_event )
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

	void FloatFieldDraggableWidget::_onTextFieldEdited()
	{
		const float newValue = _textField->text().toFloat();
		if ( newValue != _value )
		{
			setValue( newValue );
			emit onValueChange( _value );
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

	void FloatFieldDraggableWidget::_refresh()
	{
		std::stringstream strStream;
		strStream << std::fixed << std::setprecision( _nbDecimals ) << _value;
		_textField->setText( QString::fromStdString( strStream.str() ) );
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
		_textField->setText( "-" );
		_textField->blockSignals( false );
	}

} // namespace VTX::UI::Widget::CustomWidget
