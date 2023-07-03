#include "ui/old_ui/ui/widget/custom_widget/vector3_widget.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"

namespace VTX::UI::Widget::CustomWidget
{
	void Vector3Widget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		const float spinBoxMinimumWidth = 50;

		QHBoxLayout * const mainLayout = new QHBoxLayout( this );
		mainLayout->setSpacing( Style::DATA_GRID_HORIZONTAL_SPACE );

		_widgetX = WidgetFactory::get().instantiateWidget<FloatFieldDraggableWidget>( this, "vector_widget_x" );
		_widgetX->setLabel( "x" );
		_widgetX->setMinimumWidth( spinBoxMinimumWidth );
		_widgetX->separateChangeAndDrag( true );

		_widgetY = WidgetFactory::get().instantiateWidget<FloatFieldDraggableWidget>( this, "vector_widget_x" );
		_widgetY->setLabel( "y" );
		_widgetY->setMinimumWidth( spinBoxMinimumWidth );
		_widgetY->separateChangeAndDrag( true );

		_widgetZ = WidgetFactory::get().instantiateWidget<FloatFieldDraggableWidget>( this, "vector_widget_x" );
		_widgetZ->setLabel( "z" );
		_widgetZ->setMinimumWidth( spinBoxMinimumWidth );
		_widgetZ->separateChangeAndDrag( true );

		mainLayout->addWidget( _widgetX );
		mainLayout->addWidget( _widgetY );
		mainLayout->addWidget( _widgetZ );
	}

	void Vector3Widget::_setupSlots()
	{
		connect( _widgetX, &FloatFieldDraggableWidget::onValueChange, this, &Vector3Widget::_onInternalValueXChanged );
		connect( _widgetY, &FloatFieldDraggableWidget::onValueChange, this, &Vector3Widget::_onInternalValueYChanged );
		connect( _widgetZ, &FloatFieldDraggableWidget::onValueChange, this, &Vector3Widget::_onInternalValueZChanged );

		connect( _widgetX, &FloatFieldDraggableWidget::onValueDragged, this, &Vector3Widget::_onInternalValueXDragged );
		connect( _widgetY, &FloatFieldDraggableWidget::onValueDragged, this, &Vector3Widget::_onInternalValueYDragged );
		connect( _widgetZ, &FloatFieldDraggableWidget::onValueDragged, this, &Vector3Widget::_onInternalValueZDragged );
	}

	const Vec3f & Vector3Widget::getData() const { return _data; };
	void		  Vector3Widget::setData( const Vec3f & p_vec3 )
	{
		_data = p_vec3;
		_refresh();
	};

	void Vector3Widget::setMin( const float p_min )
	{
		_widgetX->setMin( p_min );
		_widgetY->setMin( p_min );
		_widgetZ->setMin( p_min );
	};
	void Vector3Widget::setMin( const Vec3f p_min )
	{
		_widgetX->setMin( p_min.x );
		_widgetY->setMin( p_min.y );
		_widgetZ->setMin( p_min.z );
	};
	void Vector3Widget::setMax( const float p_max )
	{
		_widgetX->setMax( p_max );
		_widgetY->setMax( p_max );
		_widgetZ->setMax( p_max );
	};
	void Vector3Widget::setMax( const Vec3f p_min )
	{
		_widgetX->setMax( p_min.x );
		_widgetY->setMax( p_min.y );
		_widgetZ->setMax( p_min.z );
	};
	void Vector3Widget::setMinMax( const float p_min, const float p_max )
	{
		_widgetX->setMinMax( p_min, p_max );
		_widgetY->setMinMax( p_min, p_max );
		_widgetZ->setMinMax( p_min, p_max );
	}
	void Vector3Widget::setMinMax( const Vec3f p_min, const Vec3f p_max )
	{
		_widgetX->setMinMax( p_min.x, p_max.x );
		_widgetY->setMinMax( p_min.y, p_max.y );
		_widgetZ->setMinMax( p_min.z, p_max.z );
	}

	void Vector3Widget::setDragValueFactor( const float p_factor )
	{
		_widgetX->setDragValueFactor( p_factor );
		_widgetY->setDragValueFactor( p_factor );
		_widgetZ->setDragValueFactor( p_factor );
	}

	void Vector3Widget::_onInternalValueXChanged( const float p_newValue )
	{
		_data.x = p_newValue;
		emit onValueChange( _data, AxisMask::X );
	}
	void Vector3Widget::_onInternalValueYChanged( const float p_newValue )
	{
		_data.y = p_newValue;
		emit onValueChange( _data, AxisMask::Y );
	}
	void Vector3Widget::_onInternalValueZChanged( const float p_newValue )
	{
		_data.z = p_newValue;
		emit onValueChange( _data, AxisMask::Z );
	}

	void Vector3Widget::_onInternalValueXDragged( const float p_delta )
	{
		const Vec3f delta = { p_delta, 0, 0 };
		_data.x			  = p_delta;

		emit onValueDragged( delta );
	}
	void Vector3Widget::_onInternalValueYDragged( const float p_delta )
	{
		const Vec3f delta = { 0, p_delta, 0 };
		_data.y			  = p_delta;

		emit onValueDragged( delta );
	}
	void Vector3Widget::_onInternalValueZDragged( const float p_delta )
	{
		const Vec3f delta = { 0, 0, p_delta };
		_data.z			  = p_delta;

		emit onValueDragged( delta );
	}

	void Vector3Widget::_refresh()
	{
		_widgetX->setValue( _data.x );
		_widgetY->setValue( _data.y );
		_widgetZ->setValue( _data.z );
	}

	void Vector3Widget::localize() {};

	void Vector3Widget::resetState()
	{
		TMultiDataField::resetState();

		_widgetX->resetState();
		_widgetY->resetState();
		_widgetZ->resetState();
	}

	void Vector3Widget::updateWithNewValue( const Vec3f & p_value )
	{
		_widgetX->updateWithNewValue( p_value.x );
		_widgetY->updateWithNewValue( p_value.y );
		_widgetZ->updateWithNewValue( p_value.z );

		if ( !_widgetX->hasIdenticalData() && !_widgetY->hasIdenticalData() && !_widgetZ->hasIdenticalData() )
		{
			_state = MultiDataField::State::Different;
		}
		else
		{
			_data = p_value;
		}
	}

	void Vector3Widget::_displayDifferentsDataFeedback() {}

} // namespace VTX::UI::Widget::CustomWidget
