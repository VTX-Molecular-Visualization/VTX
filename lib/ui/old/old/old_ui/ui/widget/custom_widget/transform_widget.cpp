#include "ui/old_ui/ui/widget/custom_widget/transform_widget.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include <QGridLayout>
#include <QLabel>
#include <app/old/application/setting.hpp>

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
		_positionWidget->setMinMax( VTX::App::Old::Application::Setting::MIN_SCENE_POS,
									VTX::App::Old::Application::Setting::MAX_SCENE_POS );

		_rotationLabel = new QLabel();
		_rotationLabel->setText( "Rotation" );
		_rotationWidget = WidgetFactory::get().instantiateWidget<Vector3Widget>( this, "transform_rotation_widget" );
		_rotationWidget->setMinMax( VTX::App::Old::Application::Setting::MIN_EULER,
									VTX::App::Old::Application::Setting::MAX_EULER );

		_scaleLabel = new QLabel();
		_scaleLabel->setText( "Scale" );
		_scaleWidget = WidgetFactory::get().instantiateWidget<Vector3Widget>( this, "transform_scale_widget" );
		_scaleWidget->setMinMax( VTX::App::Old::Application::Setting::MIN_SCALE, VTX::App::Old::Application::Setting::MAX_SCALE );
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

	void TransformWidget::_onPositionChange( const Vec3f & p_position, const Vector3Widget::AxisMask & p_axisMask )
	{
		Vec3f newPosition = _applyVectorWithMask( _transform.getTranslationVector(), p_position, p_axisMask );

		_transform.setTranslation( newPosition );
		const App::Old::Internal::Math::TRANSFORM_COMPOSANT_MASK mask
			= _generateTransformMask( Field::Position, p_axisMask );
		emit onValueChange( _transform, mask );
	}
	void TransformWidget::_onRotationChange( const Vec3f & p_euler, const Vector3Widget::AxisMask & p_axisMask )
	{
		Vec3f newEuler = _applyVectorWithMask( _transform.getTranslationVector(), p_euler, p_axisMask );

		_transform.setRotation( newEuler );
		const App::Old::Internal::Math::TRANSFORM_COMPOSANT_MASK mask = _generateTransformMask( Field::Euler, p_axisMask );
		emit												onValueChange( _transform, mask );
	}
	void TransformWidget::_onScaleChange( const Vec3f & p_scale, const Vector3Widget::AxisMask & p_axisMask )
	{
		Vec3f newScale = _applyVectorWithMask( _transform.getTranslationVector(), p_scale, p_axisMask );

		_transform.setScale( newScale );
		const App::Old::Internal::Math::TRANSFORM_COMPOSANT_MASK mask = _generateTransformMask( Field::Scale, p_axisMask );
		emit												onValueChange( _transform, mask );
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

	void TransformWidget::updateWithNewValue( const App::Old::Internal::Math::Transform & p_value )
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

	Vec3f TransformWidget::_applyVectorWithMask( const Vec3f &					 p_base,
												 const Vec3f &					 p_vector,
												 const Vector3Widget::AxisMask & p_mask )
	{
		Vec3f res = p_base;

		if ( p_mask & Vector3Widget::AxisMask::X )
			res.x = p_vector.x;
		if ( p_mask & Vector3Widget::AxisMask::Y )
			res.y = p_vector.y;
		if ( p_mask & Vector3Widget::AxisMask::Z )
			res.z = p_vector.z;

		return res;
	}

	App::Old::Internal::Math::TRANSFORM_COMPOSANT_MASK TransformWidget::_generateTransformMask(
		const Field &					p_field,
		const Vector3Widget::AxisMask & p_axis )
	{
		App::Old::Internal::Math::TRANSFORM_COMPOSANT_MASK res = App::Old::Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::NONE;

		switch ( p_field )
		{
		case Field::Position:
			if ( p_axis & Vector3Widget::AxisMask::X )
			{
				res |= App::Old::Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::TRANSLATE_X;
			}
			if ( p_axis & Vector3Widget::AxisMask::Y )
			{
				res |= App::Old::Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::TRANSLATE_Y;
			}
			if ( p_axis & Vector3Widget::AxisMask::Z )
			{
				res |= App::Old::Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::TRANSLATE_Z;
			}

			break;

		case Field::Euler:
			if ( p_axis & Vector3Widget::AxisMask::X )
			{
				res |= App::Old::Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::EULER_X;
			}
			if ( p_axis & Vector3Widget::AxisMask::Y )
			{
				res |= App::Old::Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::EULER_Y;
			}
			if ( p_axis & Vector3Widget::AxisMask::Z )
			{
				res |= App::Old::Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::EULER_Z;
			}

			break;

		case Field::Scale:
			if ( p_axis & Vector3Widget::AxisMask::X )
			{
				res |= App::Old::Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::SCALE_X;
			}
			if ( p_axis & Vector3Widget::AxisMask::Y )
			{
				res |= App::Old::Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::SCALE_Y;
			}
			if ( p_axis & Vector3Widget::AxisMask::Z )
			{
				res |= App::Old::Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::SCALE_Z;
			}

			break;

		default:
			VTX_ERROR( "Unknown transform mask in TransformWidget::_generateTransformMask ( {} ).", int( p_field ) );
			break;
		}

		return res;
	}

} // namespace VTX::UI::Widget::CustomWidget