#include "ui/internal/animation/reset_camera.hpp"
#include "ui/helper/animation.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/controller/base_camera_controller.hpp"
#include "ui/qt/controller/base_controller.hpp"
#include "ui/qt/controller/base_picker_controller.hpp"
#include "ui/qt/controller/camera_animation_controller.hpp"
#include "ui/qt/mode/visualization.hpp"
#include <app/application/scene.hpp>
#include <app/application/system/ecs_system.hpp>
#include <app/component/scene/transform_component.hpp>
#include <app/internal/constants.hpp>

namespace VTX::UI::Internal::Animation
{
	ResetInfo::ResetInfo( App::Component::Render::Camera & p_camera, const float p_duration ) : camera( p_camera )
	{
		App::Component::Scene::Transform & cameraTransformComponent
			= App::MAIN_REGISTRY().getComponent<App::Component::Scene::Transform>( p_camera );

		const Util::Math::AABB sceneAABB = App::SCENE().getAABB();

		_translationInfo.startPosition = cameraTransformComponent.getTransform().getTranslationVector();
		_translationInfo.startRotation = Util::Math::toQuat( cameraTransformComponent.getTransform().getRotation() );

		_translationInfo.finalPosition = UI::Helper::Animation::computeCameraOrientPosition(
			App::Internal::CAMERA_FRONT_DEFAULT, p_camera.getFov(), sceneAABB
		);

		_translationInfo.finalRotation = QUATF_ID;

		_translationInfo.targetPtr = &cameraTransformComponent;
		_translationInfo.duration  = p_duration;

		targetStartPosition = p_camera.getTarget();
		targetFinalPosition = sceneAABB.centroid();
	}

	ResetCamera::ResetCamera( const ResetInfo & p_info ) :
		Translation( p_info.getTranslationInfo() ), _resetInfo( p_info ), _camera( p_info.camera )
	{
	}

	void ResetCamera::_enter()
	{
		Translation::_enter();

		if ( !isRunning() )
			return;

		if ( QT::MODE().getName() == QT::Mode::Visualization::ID )
		{
			QT::Mode::Visualization & visualizationMode = dynamic_cast<QT::Mode::Visualization &>( QT::MODE() );
			visualizationMode.getCurrentCameraController().setActive( false );
			visualizationMode.getCurrentPickerController().setActive( false );
		}
	}

	void ResetCamera::_exit()
	{
		Translation::_exit();

		// Update camera target position.
		const Vec3f target = Util::Math::easeInOutInterpolation(
			_resetInfo.targetStartPosition, _resetInfo.targetFinalPosition, getRatio()
		);

		_camera.setTargetWorld( target );

		if ( QT::MODE().getName() == QT::Mode::Visualization::ID )
		{
			QT::Mode::Visualization & visualizationMode = dynamic_cast<QT::Mode::Visualization &>( QT::MODE() );
			visualizationMode.getCurrentCameraController().setActive( true );
			visualizationMode.getCurrentPickerController().setActive( true );
		}
	}
} // namespace VTX::UI::Internal::Animation
