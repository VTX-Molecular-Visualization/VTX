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
		positionLabel->setText( "position" );
		_positionWidget = WidgetFactory::get().instantiateWidget<Vector3Widget>( this, "transform_position_widget" );
		_positionWidget->setSpinBoxMinMax( -10000, 10000 );
		_positionWidget->setSpinBoxStep( 1 );

		QLabel * const rotationLabel = new QLabel();
		rotationLabel->setText( "rotation" );
		_rotationWidget = WidgetFactory::get().instantiateWidget<Vector3Widget>( this, "transform_rotation_widget" );
		_rotationWidget->setSpinBoxMinMax( -10000, 10000 );
		_rotationWidget->setSpinBoxStep( 5 );

		QLabel * const scaleLabel = new QLabel();
		scaleLabel->setText( "scale" );
		_scaleWidget = WidgetFactory::get().instantiateWidget<Vector3Widget>( this, "transform_scale_widget" );
		_scaleWidget->setSpinBoxMinMax( 0.01, 10000 );
		_scaleWidget->setSpinBoxStep( 0.2 );

		mainLayout->addWidget( positionLabel, 0, 0 );
		mainLayout->addWidget( _positionWidget, 0, 1 );
		mainLayout->addWidget( rotationLabel, 1, 0 );
		mainLayout->addWidget( _rotationWidget, 1, 1 );
		mainLayout->addWidget( scaleLabel, 2, 0 );
		mainLayout->addWidget( _scaleWidget, 2, 1 );
	}

	void TransformWidget::_setupSlots()
	{
		connect( _positionWidget,
				 QOverload<const Vec3f &>::of( &Vector3Widget::onValueChange ),
				 this,
				 &TransformWidget::_onPositionChange );
		connect( _rotationWidget,
				 QOverload<const Vec3f &>::of( &Vector3Widget::onValueChange ),
				 this,
				 &TransformWidget::_onRotationChange );
		connect( _scaleWidget,
				 QOverload<const Vec3f &>::of( &Vector3Widget::onValueChange ),
				 this,
				 &TransformWidget::_onScaleChange );
	}

	void TransformWidget::_refresh()
	{
		_positionWidget->setData( _transform.getTranslationVector() );
		_rotationWidget->setData( _transform.getEulerAngles() );
		_scaleWidget->setData( _transform.getScaleVector() );
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
} // namespace VTX::UI::Widget::CustomWidget
