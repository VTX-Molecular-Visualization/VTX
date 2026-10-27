#include "ui/qt/widget/camera.hpp"
#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/helper.hpp"
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
	Camera::Camera( const Entity p_entity, QWidget * const p_parent ) :
		QGroupBox( "Camera", p_parent ), _entity( p_entity )
	{
		// Layout.
		auto * layout = Helper::makeQObject<QVBoxLayout>( this );

		// Projection.
		auto * labelProjection	 = Helper::makeQObject<QLabel>( "Projection", this );
		auto * toolbarProjection = Helper::makeQObject<ExpandableToolBar>( this );
		toolbarProjection->addAction( Action::Camera::PERSPECTIVE );
		toolbarProjection->addAction( Action::Camera::ORTHOGRAPHIC );
		layout->addWidget( labelProjection );
		layout->addWidget( toolbarProjection );

		// Fov.
		auto * labelFov = Helper::makeQObject<QLabel>( "Field of view", this );
		_sliderFov		= Helper::makeQObject<EditableSlider>( Qt::Orientation::Horizontal, this );
		_sliderFov->setMinimum( Renderer::FOV_MIN );
		_sliderFov->setMaximum( Renderer::FOV_MAX );
		layout->addWidget( labelFov );
		layout->addWidget( _sliderFov );

		// Near.
		auto * labelNear = Helper::makeQObject<QLabel>( "Near plane", this );
		_sliderNear		 = Helper::makeQObject<EditableSlider>( Qt::Orientation::Horizontal, this );
		_sliderNear->setMinimum( Renderer::NEAR_CLIP_MIN );
		_sliderNear->setMaximum( Renderer::NEAR_CLIP_MAX );
		layout->addWidget( labelNear );
		layout->addWidget( _sliderNear );

		// Far.
		auto * labelFar = Helper::makeQObject<QLabel>( "Far plane", this );
		_sliderFar		= Helper::makeQObject<EditableSlider>( Qt::Orientation::Horizontal, this );
		_sliderFar->setMinimum( Renderer::FAR_CLIP_MIN );
		_sliderFar->setMaximum( Renderer::FAR_CLIP_MAX );
		layout->addWidget( labelFar );
		layout->addWidget( _sliderFar );

		// Transform widget.
		_transform = Helper::makeQObject<Transform>( this, Transform::E_FLAG::POSITION | Transform::E_FLAG::ROTATION );
		layout->addWidget( _transform );

		// Save viewpoint button.
		auto * buttonSaveViewpoint = Helper::makeQObject<ActionablePushButton>(
			UI_ACTIONS().getAction( Action::Camera::SAVE_VIEWPOINT ), this
		);
		layout->addWidget( buttonSaveViewpoint );

		// Connect.
		connect(
			_transform,
			&Transform::positionChanged,
			[]( const Vec3f & p_position ) { App::ACTION().execute<App::Action::Camera::SetPosition>( p_position ); }
		);
		connect(
			_transform,
			&Transform::rotationChanged,
			[]( const Quatf & p_rotation ) { App::ACTION().execute<App::Action::Camera::SetRotation>( p_rotation ); }
		);
		connect(
			_sliderFov,
			&EditableSlider::valueChanged,
			[]( const float p_value ) { App::ACTION().execute<App::Action::Camera::SetFov>( p_value ); }
		);
		connect(
			_sliderNear,
			&EditableSlider::valueChanged,
			[]( const float p_value ) { App::ACTION().execute<App::Action::Camera::SetNearClip>( p_value ); }
		);
		connect(
			_sliderFar,
			&EditableSlider::valueChanged,
			[]( const float p_value ) { App::ACTION().execute<App::Action::Camera::SetFarClip>( p_value ); }
		);

		// Connect app.
		_transformChangedConnection
			= App::HUB().connect<App::Events::CameraTransformChange, &Camera::_setTransform>( this );
		_cameraUpdatedConnection = App::REG().on_update<Renderer::Camera>().connect<&Camera::_setCamera>( this );

		// Set values.
		_setTransform();
		_setCamera();
	}

	void Camera::_setTransform()
	{
		const QSignalBlocker blocker( _transform );

		auto & transform = App::REG().get<Util::Math::Transform>( _entity );
		_transform->setTransform( transform );
	}

	void Camera::_setCamera()
	{
		const QSignalBlocker blocker1( _sliderFov );
		const QSignalBlocker blocker2( _sliderNear );
		const QSignalBlocker blocker3( _sliderFar );

		auto & camera = App::REG().get<Renderer::Camera>( _entity );
		_sliderFov->setValue( camera.fov );
		_sliderNear->setValue( camera.near );
		_sliderFar->setValue( camera.far );
	}
} // namespace VTX::UI::QT::Widget
