#include "app/action/camera.hpp"
#include "app/action/action_manager.hpp"
#include "app/helper/system.hpp"
#include "app/scene/tag_root.hpp"
#include "app/system/selection.hpp"
#include "app/system/trajectory.hpp"
#include <core/struct/system.hpp>
#include <util/event_hub.hpp>
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
		const auto [ _, camera, transform ]
			= ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();
		transform.setPosition( p_position );
		HUB().trigger<Events::CameraTransformChange>();
	}

	void SetRotation::execute( const Vec3f & p_eulerAngles )
	{
		const auto [ _, camera, transform ]
			= ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();
		transform.setRotation( p_eulerAngles );
		HUB().trigger<Events::CameraTransformChange>();
	}

	void SetFov::execute( const float p_fov )
	{
		const auto	entity	   = ECS::getFirstEntityOnlyWithComponents<Renderer::Camera>();
		const float clampedFov = std::clamp( p_fov, Renderer::FOV_MIN, Renderer::FOV_MAX );
		REG().patch<Renderer::Camera>( entity, [ clampedFov ]( Renderer::Camera & p_cam ) { p_cam.fov = clampedFov; } );
	}

	void SetNearClip::execute( const float p_near )
	{
		const auto	entity		= ECS::getFirstEntityOnlyWithComponents<Renderer::Camera>();
		const float clampedNear = std::clamp( p_near, Renderer::NEAR_CLIP_MIN, Renderer::NEAR_CLIP_MAX );
		REG().patch<Renderer::Camera>(
			entity, [ clampedNear ]( Renderer::Camera & p_cam ) { p_cam.near = clampedNear; }
		);
	}

	void SetFarClip::execute( const float p_far )
	{
		const auto	entity	   = ECS::getFirstEntityOnlyWithComponents<Renderer::Camera>();
		const float clampedFar = std::clamp( p_far, Renderer::FAR_CLIP_MIN, Renderer::FAR_CLIP_MAX );
		REG().patch<Renderer::Camera>( entity, [ clampedFar ]( Renderer::Camera & p_cam ) { p_cam.far = clampedFar; } );
	}

	void Reset::execute()
	{
		const auto	 entScene = ECS::getFirstEntityOnlyWithComponents<App::Scene::TagRoot, Util::Math::AABB>();
		const auto & aabb	  = REG().get<Util::Math::AABB>( entScene );
		const auto [ _, camera, transform ]
			= ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();

		Vec3f position = _computeCameraOrientPosition( FRONT_AXIS, camera.fov, aabb );
		transform.setPosition( position );
		transform.setRotation( QUATF_ID );
		transform.lookAt( aabb.centroid() );
		camera.target = aabb.centroid();
		HUB().trigger<Events::CameraTransformChange>();
	}

	void Orient::execute()
	{
		Util::Math::AABB aabb;

		// From selection.
		auto view = ECS::registry().view<Core::Struct::System, Util::Math::AABB, System::Selection>();

		if ( view.size_hint() )
		{
			view.each(
				[ & ](
					const ECS::Entity &			 p_e,
					const Core::Struct::System & p_data,
					const Util::Math::AABB &	 p_aabb,
					const System::Selection &	 p_selection
				)
				{
					if ( Helper::System::isFullySelected<Core::Struct::E_SYSTEM_ITEM::SYSTEM>( p_e ) )
					{
						aabb.extend( p_aabb );
					}
					// TODO: not recompute each time: cache values?
					else
					{
						std::span<const Vec3f> atomPositions = System::getCurrentAtomPositions( p_e );

						for ( auto atomIndex : p_selection.atoms )
						{
							aabb.extend( atomPositions[ atomIndex ], Core::ChemDB::Atom::VDW_RADIUS_MIN );
						}
					}
				}
			);
		}

		// From scene.
		if ( not aabb.isValid() )
		{
			auto entScene = ECS::getFirstEntityOnlyWithComponents<App::Scene::TagRoot, Util::Math::AABB>();
			aabb		  = REG().get<Util::Math::AABB>( entScene );
		}

		execute( aabb );
	}

	void Orient::execute( const Util::Math::AABB & p_target )
	{
		using namespace Util;

		const auto [ _, camera, transform ]
			= ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();
		camera.target = p_target.centroid();
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
		// TODO: controller target?
		ACTION().execute<Animate<E_CAMERA_INTERPOLATOR::EASE_IN_OUT>>( p_targetPosition, p_targetRotation, p_duration );
	}

} // namespace VTX::App::Action::Camera
