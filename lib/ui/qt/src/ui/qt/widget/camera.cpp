#include "ui/qt/widget/camera.hpp"
#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/widget/actionable_push_button.hpp"
#include "ui/qt/widget/expandable_tool_bar.hpp"
#include <QLabel>
#include <app/action/action_manager.hpp>
#include <app/action/camera.hpp>
#include <renderer/camera.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::Widget
{
	Camera::Camera( const Entity p_entity, QWidget * p_parent ) : QGroupBox( "Camera", p_parent ), _entity( p_entity )
	{
		// Layout.
		auto * layout = new QVBoxLayout( this );

		// Projection.
		auto * labelProjection	 = new QLabel( "Projection", this );
		auto * toolbarProjection = new ExpandableToolBar( this );
		toolbarProjection->addAction( Action::Camera::PERSPECTIVE );
		toolbarProjection->addAction( Action::Camera::ORTHOGRAPHIC );
		layout->addWidget( labelProjection );
		layout->addWidget( toolbarProjection );

		// Controller.
		auto * labelController	 = new QLabel( "Controller", this );
		auto * toolbarController = new ExpandableToolBar( this );
		toolbarController->addAction( Action::Camera::TRACKBALL );
		toolbarController->addAction( Action::Camera::FREEFLY );
		layout->addWidget( labelController );
		layout->addWidget( toolbarController );

		// Fov.
		auto * labelFov = new QLabel( "Field of view", this );
		_sliderFov		= new EditableSlider( Qt::Orientation::Horizontal, this );
		_sliderFov->setMinimum( Renderer::FOV_MIN );
		_sliderFov->setMaximum( Renderer::FOV_MAX );
		layout->addWidget( labelFov );
		layout->addWidget( _sliderFov );

		// Near.
		auto * labelNear = new QLabel( "Near plane", this );
		_sliderNear		 = new EditableSlider( Qt::Orientation::Horizontal, this );
		_sliderNear->setMinimum( Renderer::NEAR_CLIP_MIN );
		_sliderNear->setMaximum( Renderer::NEAR_CLIP_MAX );
		layout->addWidget( labelNear );
		layout->addWidget( _sliderNear );

		// Far.
		auto * labelFar = new QLabel( "Far plane", this );
		_sliderFar		= new EditableSlider( Qt::Orientation::Horizontal, this );
		_sliderFar->setMinimum( Renderer::FAR_CLIP_MIN );
		_sliderFar->setMaximum( Renderer::FAR_CLIP_MAX );
		layout->addWidget( labelFar );
		layout->addWidget( _sliderFar );

		// Transform widget.
		_transform = new Transform( this, Transform::E_FLAG::POSITION | Transform::E_FLAG::ROTATION );
		layout->addWidget( _transform );

		// Save viewpoint button.
		auto * buttonSaveViewpoint
			= new ActionablePushButton( UI_ACTIONS().getAction( Action::Camera::SAVE_VIEWPOINT ), this );
		layout->addWidget( buttonSaveViewpoint );

		// Connect.
		connect(
			_transform,
			&Transform::positionChanged,
			[ this ]( const Vec3f & p_position )
			{ App::ACTION().execute<App::Action::Camera::SetPosition>( p_position ); }
		);
		connect(
			_transform,
			&Transform::rotationChanged,
			[ this ]( const Quatf & p_rotation )
			{ App::ACTION().execute<App::Action::Camera::SetRotation>( p_rotation ); }
		);
		connect(
			_sliderFov,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value ) { App::ACTION().execute<App::Action::Camera::SetFov>( p_value ); }
		);
		connect(
			_sliderNear,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value ) { App::ACTION().execute<App::Action::Camera::SetNearClip>( p_value ); }
		);
		connect(
			_sliderFar,
			&EditableSlider::valueChanged,
			[ this ]( const float p_value ) { App::ACTION().execute<App::Action::Camera::SetFarClip>( p_value ); }
		);

		// Connect app.
		App::HUB().connect<App::Events::CameraTransformChange, &Camera::_setTransform>( this );
		App::REG().on_update<Renderer::Camera>().connect<&Camera::_setCamera>( this );

		// Set values.
		const auto [ _, camera, transform ]
			= App::ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();
		_setTransform();
		_setCamera();
	}

	Camera::~Camera()
	{
		App::HUB().disconnectAllOf( *this );
		App::REG().on_update<Renderer::Camera>().disconnect<&Camera::_setCamera>( this );
	}

	void Camera::_setTransform()
	{
		QSignalBlocker blocker( _transform );

		auto & transform = App::REG().get<Util::Math::Transform>( _entity );
		_transform->setTransform( transform );
	}

	void Camera::_setCamera()
	{
		QSignalBlocker blocker1( _sliderFov );
		QSignalBlocker blocker2( _sliderNear );
		QSignalBlocker blocker3( _sliderFar );

		auto & camera = App::REG().get<Renderer::Camera>( _entity );
		_sliderFov->setValue( camera.fov );
		_sliderNear->setValue( camera.near );
		_sliderFar->setValue( camera.far );
	}
} // namespace VTX::UI::QT::Widget
