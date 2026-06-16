#include "ui/qt/widget/viewpoint.hpp"
#include <QGroupBox>
#include <QPushButton>
#include <QSignalBlocker>
#include <app/action/action_manager.hpp>
#include <app/action/camera.hpp>
#include <app/services.hpp>
#include <util/math/transform.hpp>

namespace VTX::UI::QT::Widget
{
	ViewPoint::ViewPoint( const Entity p_entity, QWidget * const p_parent ) : QWidget( p_parent ), _entity( p_entity )
	{
		_layout = new QVBoxLayout( this );
		_layout->setContentsMargins( 0, 0, 0, 0 );

		auto * groupBoxTransform = new QGroupBox( "Viewpoint", this );
		auto * layoutTransform   = new QVBoxLayout( groupBoxTransform );

		_transform = new Transform( this, Transform::E_FLAG::POSITION | Transform::E_FLAG::ROTATION );
		layoutTransform->addWidget( _transform );
		_layout->addWidget( groupBoxTransform );

		auto * buttonUpdateFromCamera = new QPushButton( "Update from current camera", this );
		_layout->addWidget( buttonUpdateFromCamera );

		connect(
			_transform,
			&Transform::positionChanged,
			[ this ]( const Vec3f & p_position )
			{ App::ACTION().execute<App::Action::Camera::SetViewPointPosition>( _entity, p_position ); }
		);
		connect(
			_transform,
			&Transform::rotationChanged,
			[ this ]( const Quatf & p_rotation )
			{ App::ACTION().execute<App::Action::Camera::SetViewPointRotation>( _entity, p_rotation ); }
		);
		connect(
			buttonUpdateFromCamera,
			&QPushButton::clicked,
			[ this ]() { App::ACTION().execute<App::Action::Camera::UpdateViewPointFromCamera>( _entity ); }
		);

		_connTransformChanged
			= App::REG().on_update<Util::Math::Transform>().connect<&ViewPoint::_transformUpdated>( this );

		_setTransform();
	}

	ViewPoint::~ViewPoint() { _connTransformChanged.release(); }

	void ViewPoint::_setTransform()
	{
		QSignalBlocker blocker( _transform );
		_transform->setTransform( App::REG().get<Util::Math::Transform>( _entity ) );
	}

	void ViewPoint::_transformUpdated( Registry &, const Entity p_entity )
	{
		if ( p_entity != _entity )
		{
			return;
		}

		_setTransform();
	}
} // namespace VTX::UI::QT::Widget
