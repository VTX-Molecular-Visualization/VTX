#include "app/animation/orient.hpp"
#include "app/mode/visualization.hpp"
#include <app/application/system/ecs_system.hpp>

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
									.getRotation() ),
			p_aabb.centroid()
		)
	{
		// Set interpolation functions.
		_positionFunc = []( const Vec3f & lhs, const Vec3f & rhs, float value )
		{ return Util::Math::easeInOutInterpolation<Vec3f, float>( lhs, rhs, value ); };

		_rotationFunc = []( const Quatf & lhs, const Quatf & rhs, float value )
		{ return Util::Math::easeInOutInterpolation<Quatf, float>( lhs, rhs, value ); };
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

} // namespace VTX::App::Animation
