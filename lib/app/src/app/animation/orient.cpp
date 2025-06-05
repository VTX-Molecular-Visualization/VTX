#include "app/animation/orient.hpp"
#include "app/core/ecs/ecs_system.hpp"
#include "app/mode/visualization.hpp"

namespace VTX::App::Animation
{
	Orient::Orient( const App::Component::Render::Camera & p_camera, const Util::Math::AABB & p_aabb ) :
		_animation(
			App::ECS_REGISTRY().getComponent<App::Component::Scene::Transform>( p_camera ).getTransform(),
			Orient::computeCameraOrientPosition(
				p_camera.getTransform().getFront(),
				p_camera.getFov(),
				p_aabb,
				ORIENT_ZOOM_FACTOR
			),
			Util::Math::toQuat(
				App::ECS_REGISTRY()
					.getComponent<App::Component::Scene::Transform>( p_camera )
					.getTransform()
					.getRotation()
			),
			p_aabb.centroid()
		)
	{
		// Set interpolation functions.
		_animation.setPositionFunc( []( const Vec3f & lhs, const Vec3f & rhs, float value )
									{ return Util::Math::easeInOutInterpolation<Vec3f, float>( lhs, rhs, value ); } );

		_animation.setRotationFunc( []( const Quatf & lhs, const Quatf & rhs, float value )
									{ return Util::Math::easeInOutInterpolation<Quatf, float>( lhs, rhs, value ); } );
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

	void Orient::update( const float p_delta, const float p_elasped )
	{
		//
		_animation.update( p_delta, p_elasped );
		//
	}

	void Orient::play() { _animation.play(); }

	void Orient::stop() { _animation.stop(); }

	float Orient::getRatio() const { return _animation.getRatio(); }

} // namespace VTX::App::Animation
