#include "app/action/camera.hpp"
#include "app/action/action_manager.hpp"
#include "app/scene/tag_root.hpp"
#include <util/math/transform.hpp>

namespace
{
	using namespace VTX;

	constexpr float _ORIENT_ZOOM_FACTOR = 0.666f;

	Vec3f _computeCameraOrientPosition(
		const Vec3f				 p_forward,
		const float				 p_fov,
		const Util::Math::AABB & p_target,
		const float				 p_zoomFactor = _ORIENT_ZOOM_FACTOR
	)
	{
		const float orientTargetDistance = p_target.radius() / std::tan( Util::Math::radians( p_fov ) * p_zoomFactor );
		return p_target.centroid() - ( p_forward * orientTargetDistance );
	}
} // namespace

namespace VTX::App::Action::Camera
{
	void SetPosition::execute( const Vec3f & p_position )
	{
		auto [ ent, _, transform ] = ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();
		REG().patch<Util::Math::Transform>(
			ent, [ p_position ]( Util::Math::Transform & p_transform ) { p_transform.setPosition( p_position ); }
		);
	}

	void SetRotation::execute( const Vec3f & p_eulerAngles )
	{
		auto [ ent, _, transform ] = ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();
		REG().patch<Util::Math::Transform>(
			ent, [ p_eulerAngles ]( Util::Math::Transform & p_transform ) { p_transform.setRotation( p_eulerAngles ); }
		);
	}

	void SetScale::execute( const float p_scale )
	{
		auto [ ent, _, transform ] = ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();
		REG().patch<Util::Math::Transform>(
			ent, [ p_scale ]( Util::Math::Transform & p_transform ) { p_transform.setScale( p_scale ); }
		);
	}

	void Reset::execute()
	{
		auto   entScene = ECS::getFirstEntityOnlyWithComponents<App::Scene::TagRoot, Util::Math::AABB>();
		auto & aabb		= REG().get<Util::Math::AABB>( entScene );
		auto [ entCamera, camera, transform ]
			= ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();
		REG().patch<Util::Math::Transform>(
			entCamera,
			[ & ]( Util::Math::Transform & p_transform )
			{
				Vec3f position = _computeCameraOrientPosition( p_transform.getFront(), camera.fov, aabb );

				p_transform.setPosition( position );
				p_transform.setRotation( QUATF_ID );
				p_transform.lookAt( aabb.centroid() );
			}
		);
	}

	void Orient::execute()
	{
		auto [ entCamera, camera, transform ]
			= ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();

		bool selection = false;
		// TODO: compute aabb from selection.
		if ( selection ) {}
		// From scene.
		else
		{
			auto   entScene = ECS::getFirstEntityOnlyWithComponents<App::Scene::TagRoot, Util::Math::AABB>();
			auto & aabb		= REG().get<Util::Math::AABB>( entScene );
			execute( aabb );
		}
	}

	void Orient::execute( const Util::Math::AABB & p_target )
	{
		using namespace Util;

		auto [ _, camera, transform ] = ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();

		ACTION().execute<Animate<E_CAMERA_INTERPOLATOR::EASE_IN_OUT>>(
			_computeCameraOrientPosition( transform.getFront(), camera.fov, p_target ), transform.getRotation()
		);
	}

	void StraightTravel::execute(
		const Vec3f & p_targetPosition,
		const Quatf & p_targetRotation,
		const float	  p_duration
	)
	{
		ACTION().execute<Animate<E_CAMERA_INTERPOLATOR::EASE_IN_OUT>>( p_targetPosition, p_targetRotation, p_duration );
	}

} // namespace VTX::App::Action::Camera
