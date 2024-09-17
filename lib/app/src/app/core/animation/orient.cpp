#include "app/core/animation/orient.hpp"
#include "app/core/animation/helper.hpp"
#include "app/mode/visualization.hpp"
#include <app/application/system/ecs_system.hpp>
#include <app/component/scene/transform_component.hpp>

namespace VTX::App::Core::Animation
{
	const float OrientInfo::ORIENT_ZOOM_FACTOR = 0.666f;

	OrientInfo::OrientInfo(
		App::Component::Render::Camera & p_camera,
		const float						 p_duration,
		const Util::Math::AABB &		 p_targetAabb
	) : camera( p_camera )
	{
		App::Component::Scene::Transform & cameraTransformComponent
			= App::MAIN_REGISTRY().getComponent<App::Component::Scene::Transform>( p_camera );

		_translationInfo.startPosition = cameraTransformComponent.getTransform().getTranslationVector();
		_translationInfo.startRotation = Util::Math::toQuat( cameraTransformComponent.getTransform().getRotation() );

		_translationInfo.finalPosition = Helper::computeCameraOrientPosition(
			p_camera.getTransform().getFront(), p_camera.getFov(), p_targetAabb, ORIENT_ZOOM_FACTOR
		);
		_translationInfo.finalRotation = _translationInfo.startRotation;

		_translationInfo.targetPtr = &cameraTransformComponent;
		_translationInfo.duration  = p_duration;

		targetStartPosition = p_camera.getTarget();
		targetFinalPosition = p_targetAabb.centroid();
	}

	OrientInfo::OrientInfo(
		App::Component::Render::Camera & p_camera,
		const float						 p_duration,
		const Vec3f &					 p_finalPosition,
		const Quatf &					 p_finalRotation,
		const Vec3f &					 p_targetPosition
	) : camera( p_camera )
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

		// if ( MODE().getName() == Mode::Visualization::ID )
		{
			// Mode::Visualization & visualizationMode = dynamic_cast<Mode::Visualization &>( MODE() );
			// visualizationMode.getCurrentCameraController().setActive( false );
			// visualizationMode.getCurrentPickerController().setActive( false );
		}
	}

	void Orient::_exit()
	{
		Translation::_exit();

		// Update camera target position.
		const Vec3f target = Util::Math::easeInOutInterpolation(
			_orientInfo.targetStartPosition, _orientInfo.targetFinalPosition, getRatio()
		);

		_camera.setTargetWorld( target );

		// if ( MODE().getName() == Mode::Visualization::ID )
		{
			// Mode::Visualization & visualizationMode = dynamic_cast<Mode::Visualization &>( MODE() );
			// visualizationMode.getCurrentCameraController().setActive( true );
			// visualizationMode.getCurrentPickerController().setActive( true );
		}
	}
} // namespace VTX::App::Core::Animation
