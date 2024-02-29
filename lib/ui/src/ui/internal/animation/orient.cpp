#include "ui/internal/animation/orient.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/controller/base_camera_controller.hpp"
#include "ui/qt/controller/base_controller.hpp"
#include "ui/qt/controller/base_picker_controller.hpp"
#include "ui/qt/controller/camera_animation_controller.hpp"
#include "ui/qt/mode/visualization.hpp"
#include <app/application/system/ecs_system.hpp>
#include <app/component/scene/transform_component.hpp>

namespace VTX::UI::Internal::Animation
{
	const float OrientInfo::ORIENT_ZOOM_FACTOR = 0.666f;

	OrientInfo::OrientInfo(
		App::Component::Render::Camera & p_camera,
		const float						 p_duration,
		const Util::Math::AABB &		 p_targetAabb
	) :
		camera( p_camera )
	{
		App::Component::Scene::Transform & cameraTransformComponent
			= App::MAIN_REGISTRY().getComponent<App::Component::Scene::Transform>( p_camera );

		_translationInfo.startPosition = cameraTransformComponent.getTransform().getTranslationVector();
		_translationInfo.startRotation = Util::Math::toQuat( cameraTransformComponent.getTransform().getRotation() );

		const float orientTargetDistance
			= p_targetAabb.radius()
			  / float( std::tan( Util::Math::radians( p_camera.getFov() ) * ORIENT_ZOOM_FACTOR ) );

		_translationInfo.finalPosition
			= p_targetAabb.centroid() + ( -p_camera.getTransform().getFront() * orientTargetDistance );
		_translationInfo.finalRotation = _translationInfo.startRotation;

		_translationInfo.targetPtr = &cameraTransformComponent;
		_translationInfo.duration  = p_duration;
	}

	OrientInfo::OrientInfo(
		App::Component::Render::Camera & p_camera,
		const float						 p_duration,
		const Vec3f &					 p_finalPosition,
		const Quatf &					 p_finalRotation,
		const Vec3f &					 p_targetPosition
	) :
		camera( p_camera )
	{
		App::Component::Scene::Transform & cameraTransformComponent
			= App::MAIN_REGISTRY().getComponent<App::Component::Scene::Transform>( p_camera );

		_translationInfo.startPosition = cameraTransformComponent.getTransform().getTranslationVector();
		_translationInfo.startRotation = Util::Math::toQuat( cameraTransformComponent.getTransform().getRotation() );

		_translationInfo.finalPosition = p_finalPosition;
		_translationInfo.finalRotation = _translationInfo.startRotation;

		_translationInfo.targetPtr = &cameraTransformComponent;
		_translationInfo.duration  = p_duration;
	}

	Orient::Orient( const OrientInfo & p_info ) :
		Translation( p_info.getTranslationInfo() ), _orientInfo( p_info ), _camera( p_info.camera )
	{
	}

	void Orient::_enter()
	{
		Translation::_enter();

		if ( !isRunning() )
			return;

		if ( QT::MODE().getName() == QT::Mode::Visualization::ID )
		{
			QT::Mode::Visualization & visualizationMode = dynamic_cast<QT::Mode::Visualization &>( QT::MODE() );
			visualizationMode.getCurrentCameraController().setActive( false );
			visualizationMode.getCurrentPickerController().setActive( false );
			visualizationMode.getController( QT::Controller::CameraAnimationController::COLLECTION_ID )
				.setActive( true );
		}
	}

	void Orient::_exit()
	{
		Translation::_exit();

		// Update camera target position.
		const Vec3f target = Util::Math::easeInOutInterpolation(
			_orientInfo.targetStartPosition, _orientInfo.targetFinalPosition, getRatio()
		);
		_camera.setTarget( target );

		if ( QT::MODE().getName() == QT::Mode::Visualization::ID )
		{
			QT::Mode::Visualization & visualizationMode = dynamic_cast<QT::Mode::Visualization &>( QT::MODE() );
			visualizationMode.getCurrentCameraController().setActive( true );
			visualizationMode.getCurrentPickerController().setActive( true );
			visualizationMode.getController( QT::Controller::CameraAnimationController::COLLECTION_ID )
				.setActive( false );
		}
	}
} // namespace VTX::UI::Internal::Animation
