#include "vector3_widget.hpp"

namespace VTX::UI::Widget::CustomWidget
{
	void Vector3Widget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		const float spinBoxMinimumWidth = 50;

		QHBoxLayout * const mainLayout = new QHBoxLayout( this );
		mainLayout->setSpacing( 2 );

		_widgetLabelX = new QLabel();
		_widgetLabelX->setText( "x" );
		_widgetLabelX->setMouseTracking( true );
		_widgetX = new QDoubleSpinBox();
		_widgetX->setMinimumWidth( spinBoxMinimumWidth );

		_widgetLabelY = new QLabel();
		_widgetLabelY->setText( "y" );
		_widgetLabelY->setMouseTracking( true );
		_widgetY = new QDoubleSpinBox();
		_widgetY->setMinimumWidth( spinBoxMinimumWidth );

		_widgetLabelZ = new QLabel();
		_widgetLabelZ->setText( "z" );
		_widgetLabelZ->setMouseTracking( true );
		_widgetZ = new QDoubleSpinBox();
		_widgetZ->setMinimumWidth( spinBoxMinimumWidth );

		mainLayout->addWidget( _widgetLabelX );
		mainLayout->addWidget( _widgetX );
		mainLayout->addWidget( _widgetLabelY );
		mainLayout->addWidget( _widgetY );
		mainLayout->addWidget( _widgetLabelZ );
		mainLayout->addWidget( _widgetZ );

		setMouseTracking( true );
	}

	void Vector3Widget::_setupSlots()
	{
		connect( _widgetX,
				 QOverload<const double>::of( &QDoubleSpinBox::valueChanged ),
				 this,
				 &Vector3Widget::_onInternalValueXChanged );
		connect( _widgetY,
				 QOverload<const double>::of( &QDoubleSpinBox::valueChanged ),
				 this,
				 &Vector3Widget::_onInternalValueYChanged );
		connect( _widgetZ,
				 QOverload<const double>::of( &QDoubleSpinBox::valueChanged ),
				 this,
				 &Vector3Widget::_onInternalValueZChanged );
	}

	void Vector3Widget::paintEvent( QPaintEvent * event )
	{
		QWidget::paintEvent( event );

		// TODO find a better place to catch when the window loose focus
		if ( !isActiveWindow() && _dragTarget != DragTarget::NONE )
		{
			_dragTarget = DragTarget::NONE;
			unsetCursor();
		}
	}

	void Vector3Widget::mousePressEvent( QMouseEvent * p_event )
	{
		const QPoint globalPos = p_event->globalPos();
		_dragTarget			   = _getDragTarget( globalPos );

		if ( _dragTarget != DragTarget::NONE )
		{
			p_event->accept();
			_mousePressPos = globalPos;
			setCursor( Qt::CursorShape::BlankCursor );
		}

		BaseManualWidget::mousePressEvent( p_event );
	}

	void Vector3Widget::mouseMoveEvent( QMouseEvent * p_event )
	{
		if ( _dragTarget == DragTarget::NONE )
		{
			const DragTarget dragTarget = _getDragTarget( p_event->globalPos() );

			if ( dragTarget != DragTarget::NONE )
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

			switch ( _dragTarget )
			{
			case DragTarget::X: _widgetX->setValue( _data.x + valueDelta ); break;
			case DragTarget::Y: _widgetY->setValue( _data.y + valueDelta ); break;
			case DragTarget::Z: _widgetZ->setValue( _data.z + valueDelta ); break;
			}

			p_event->accept();
		}

		BaseManualWidget::mouseMoveEvent( p_event );
	}

	void Vector3Widget::mouseReleaseEvent( QMouseEvent * p_event )
	{
		if ( _dragTarget != DragTarget::NONE )
		{
			_dragTarget = DragTarget::NONE;
			// Set sizeHorCursor because when we release, we are hovering drag target
			setCursor( Qt::CursorShape::SizeHorCursor );
		}

		BaseManualWidget::mouseReleaseEvent( p_event );
	}

	Vector3Widget::DragTarget Vector3Widget::_getDragTarget( const QPoint & p_globalPos ) const
	{
		Vector3Widget::DragTarget res;

		if ( _isOnWidget( p_globalPos, *_widgetLabelX ) )
			res = DragTarget::X;
		else if ( _isOnWidget( p_globalPos, *_widgetLabelY ) )
			res = DragTarget::Y;
		else if ( _isOnWidget( p_globalPos, *_widgetLabelZ ) )
			res = DragTarget::Z;
		else
			res = DragTarget::NONE;

		return res;
	}

	bool Vector3Widget::_isOnWidget( const QPoint & p_globalPos, const QWidget & p_widget ) const
	{
		return p_widget.rect().contains( p_widget.mapFromGlobal( p_globalPos ) );
	}

	void Vector3Widget::_onInternalValueXChanged( const float p_newValue )
	{
		_data.x = p_newValue;
		emit onValueChange( _data );
	}
	void Vector3Widget::_onInternalValueYChanged( const float p_newValue )
	{
		_data.y = p_newValue;
		emit onValueChange( _data );
	}
	void Vector3Widget::_onInternalValueZChanged( const float p_newValue )
	{
		_data.z = p_newValue;
		emit onValueChange( _data );
	}

	void Vector3Widget::_refresh()
	{
		_widgetX->setValue( _data.x );
		_widgetY->setValue( _data.y );
		_widgetZ->setValue( _data.z );
	}

	void Vector3Widget::localize() {};
} // namespace VTX::UI::Widget::CustomWidget
