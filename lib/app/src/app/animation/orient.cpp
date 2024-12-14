#include "app/animation/orient.hpp"
#include "app/mode/visualization.hpp"
#include <app/application/system/ecs_system.hpp>
#include <app/component/scene/transform_component.hpp>

namespace VTX::App::Animation
{
	Orient::Orient( const App::Component::Render::Camera & p_camera, const Util::Math::AABB & p_aabb ) :
		BaseAnimation(
			App::ECS_REGISTRY().getComponent<App::Component::Scene::Transform>( p_camera ).getTransform(),
			Orient::computeCameraOrientPosition(
				p_camera.getTransform().getFront(),
				p_camera.getFov(),
				p_aabb,
				ORIENT_ZOOM_FACTOR
			),
			Util::Math::toQuat( App::ECS_REGISTRY()
									.getComponent<App::Component::Scene::Transform>( p_camera )
									.getTransform()
									.getRotation() )
		)
	{
	}

	Vec3f Orient::computeCameraOrientPosition(
		const Vec3f				 p_forward,
		const float				 p_fov,
		const Util::Math::AABB & p_target,
		const float				 p_zoomFactor
	)
	{
		const float orientTargetDistance = p_target.radius() / std::tan( Util::Math::radians( p_fov ) * p_zoomFactor );
		return p_target.centroid() - ( p_forward * orientTargetDistance );
	}

	void Orient::update( const float p_delta, const float p_elapsed )
	{
		if ( not _isRunning )
		{
			return;
		}

		BaseAnimation::update( p_delta, p_elapsed );

		const float ratio = getRatio();

		const Vec3f newPos = Util::Math::easeInOutInterpolation( _startPosition, _finalPosition, ratio );
		_target->setTranslation( newPos );

		const Quatf newRotation = Util::Math::easeInOutInterpolation( _startRotation, _finalRotation, ratio );
		_target->setRotation( newRotation );
	}
} // namespace VTX::App::Animation
