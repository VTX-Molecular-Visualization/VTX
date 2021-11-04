#include "transform_widget.hpp"
#include "ui/widget_factory.hpp"
#include <QGridLayout>
#include <QLabel>

namespace VTX::UI::Widget::CustomWidget
{
	void TransformWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		QGridLayout * const mainLayout = new QGridLayout( this );
		mainLayout->setSpacing( 2 );

		_positionLabel = new QLabel();
		_positionLabel->setText( "Position" );
		_positionWidget = WidgetFactory::get().instantiateWidget<Vector3Widget>( this, "transform_position_widget" );
		_positionWidget->setMinMax( -10000, 10000 );

		_rotationLabel = new QLabel();
		_rotationLabel->setText( "Rotation" );
		_rotationWidget = WidgetFactory::get().instantiateWidget<Vector3Widget>( this, "transform_rotation_widget" );
		_rotationWidget->setMinMax( -10000, 10000 );

		_scaleLabel = new QLabel();
		_scaleLabel->setText( "Scale" );
		_scaleWidget = WidgetFactory::get().instantiateWidget<Vector3Widget>( this, "transform_scale_widget" );
		_scaleWidget->setMinMax( 0.01, 10000 );
		_scaleWidget->setDragValueFactor( 0.01f );

		mainLayout->addWidget( _positionLabel, 0, 0 );
		mainLayout->addWidget( _positionWidget, 0, 1 );
		mainLayout->addWidget( _rotationLabel, 1, 0 );
		mainLayout->addWidget( _rotationWidget, 1, 1 );
		mainLayout->addWidget( _scaleLabel, 2, 0 );
		mainLayout->addWidget( _scaleWidget, 2, 1 );
	}

	void TransformWidget::_setupSlots()
	{
		connect( _positionWidget, &Vector3Widget::onValueChange, this, &TransformWidget::_onPositionChange );
		connect( _positionWidget, &Vector3Widget::onValueDragged, this, &TransformWidget::_onPositionDragged );
		connect( _rotationWidget, &Vector3Widget::onValueChange, this, &TransformWidget::_onRotationChange );
		connect( _rotationWidget, &Vector3Widget::onValueDragged, this, &TransformWidget::_onRotationDragged );
		connect( _scaleWidget, &Vector3Widget::onValueChange, this, &TransformWidget::_onScaleChange );
		connect( _scaleWidget, &Vector3Widget::onValueDragged, this, &TransformWidget::_onScaleDragged );
	}

	void TransformWidget::displayPosition( const bool p_display ) const
	{
		_positionLabel->setVisible( p_display );
		_positionWidget->setVisible( p_display );
	}
	void TransformWidget::displayRotation( const bool p_display ) const
	{
		_rotationLabel->setVisible( p_display );
		_rotationWidget->setVisible( p_display );
	}
	void TransformWidget::displayScale( const bool p_display ) const
	{
		_scaleLabel->setVisible( p_display );
		_scaleWidget->setVisible( p_display );
	}

	void TransformWidget::_refresh()
	{
		const bool oldBlockState = blockSignals( true );

		if ( _positionWidget->isVisible() )
			_positionWidget->setData( _transform.getTranslationVector() );
		if ( _rotationWidget->isVisible() )
			_rotationWidget->setData( _transform.getEulerAngles() );
		if ( _scaleWidget->isVisible() )
			_scaleWidget->setData( _transform.getScaleVector() );

		blockSignals( oldBlockState );
	}

	void TransformWidget::localize() {};

	void TransformWidget::_onPositionChange( const Vec3f & p_position )
	{
		_transform.setTranslation( p_position );
		emit onValueChange( _transform );
	}
	void TransformWidget::_onRotationChange( const Vec3f & p_euler )
	{
		_transform.setRotation( p_euler );
		emit onValueChange( _transform );
	}
	void TransformWidget::_onScaleChange( const Vec3f & p_scale )
	{
		_transform.setScale( p_scale );
		emit onValueChange( _transform );
	}

	void TransformWidget::_onPositionDragged( const Vec3f & p_delta )
	{
		_transform.translate( p_delta );
		emit onPositionDragged( p_delta );
	}
	void TransformWidget::_onRotationDragged( const Vec3f & p_delta )
	{
		_transform.setRotation( _transform.getEulerAngles() + p_delta );
		emit onRotationDragged( p_delta );
	}
	void TransformWidget::_onScaleDragged( const Vec3f & p_delta )
	{
		_transform.setScale( _transform.getScaleVector() + p_delta );
		emit onScaleDragged( p_delta );
	}

	void TransformWidget::resetState()
	{
		TMultiDataField::resetState();

		_positionWidget->resetState();
		_rotationWidget->resetState();
		_scaleWidget->resetState();
	}

	void TransformWidget::updateWithNewValue( const Math::Transform & p_value )
	{
		if ( _state == MultiDataField::State::Uninitialized )
			_transform = p_value;

		_positionWidget->updateWithNewValue( p_value.getTranslationVector() );
		_rotationWidget->updateWithNewValue( p_value.getEulerAngles() );
		_scaleWidget->updateWithNewValue( p_value.getScaleVector() );

		if ( !_positionWidget->hasIdenticalData() && !_rotationWidget->hasIdenticalData()
			 && !_scaleWidget->hasIdenticalData() )
		{
			_state = MultiDataField::State::Different;
		}
	}
	void TransformWidget::_displayDifferentsDataFeedback() {}

} // namespace VTX::UI::Widget::CustomWidget
