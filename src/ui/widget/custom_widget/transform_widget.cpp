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

		QLabel * const positionLabel = new QLabel();
		positionLabel->setText( "Position" );
		_positionWidget = WidgetFactory::get().instantiateWidget<Vector3Widget>( this, "transform_position_widget" );
		_positionWidget->setMinMax( -10000, 10000 );

		QLabel * const rotationLabel = new QLabel();
		rotationLabel->setText( "Rotation" );
		_rotationWidget = WidgetFactory::get().instantiateWidget<Vector3Widget>( this, "transform_rotation_widget" );
		_rotationWidget->setMinMax( -10000, 10000 );

		QLabel * const scaleLabel = new QLabel();
		scaleLabel->setText( "Scale" );
		_scaleWidget = WidgetFactory::get().instantiateWidget<Vector3Widget>( this, "transform_scale_widget" );
		_scaleWidget->setMinMax( 0.01, 10000 );
		_scaleWidget->setDragValueFactor( 0.01f );

		mainLayout->addWidget( positionLabel, 0, 0 );
		mainLayout->addWidget( _positionWidget, 0, 1 );
		mainLayout->addWidget( rotationLabel, 1, 0 );
		mainLayout->addWidget( _rotationWidget, 1, 1 );
		mainLayout->addWidget( scaleLabel, 2, 0 );
		mainLayout->addWidget( _scaleWidget, 2, 1 );
	}

	void TransformWidget::_setupSlots()
	{
		connect( _positionWidget, &Vector3Widget::onValueChange, this, &TransformWidget::_onPositionChange );
		connect( _rotationWidget, &Vector3Widget::onValueChange, this, &TransformWidget::_onRotationChange );
		connect( _scaleWidget, &Vector3Widget::onValueChange, this, &TransformWidget::_onScaleChange );
	}

	void TransformWidget::_refresh()
	{
		const bool oldBlockState = blockSignals( true );

		_positionWidget->setData( _transform.getTranslationVector() );
		_rotationWidget->setData( _transform.getEulerAngles() );
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
