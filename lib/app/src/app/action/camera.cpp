#include "app/action/camera.hpp"
#include "app/action/action_manager.hpp"
#include "app/generic/name.hpp"
#include "app/helper/system.hpp"
#include "app/scene/tag_root.hpp"
#include "app/system/selection.hpp"
#include "app/system/trajectory.hpp"
#include <algorithm>
#include <cmath>
#include <core/struct/topology.hpp>
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
		const Vec3f delta = p_position - transform.getPosition();
		transform.setPosition( p_position );
		camera.target += delta;
		HUB().trigger<Events::CameraTransformChange>();
	}

	void SetRotation::execute( const Quatf & p_rotation )
	{
		const auto [ _, camera, transform ]
			= ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();
		const Quatf delta = p_rotation * Util::Math::conjugate( transform.getRotation() );
		transform.setRotation( p_rotation );
		const Vec3f toTarget = camera.target - transform.getPosition();
		camera.target		 = transform.getPosition() + delta * toTarget;
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

		transform.setRotation( QUATF_ID );
		const Vec3f position = _computeCameraOrientPosition( FRONT_AXIS, camera.fov, aabb );
		transform.setPosition( position );
		transform.lookAt( aabb.centroid() );
		camera.target = aabb.centroid();
		HUB().trigger<Events::CameraTransformChange>();
	}

	void Orient::execute()
	{
		Util::Math::AABB aabb;

		// From selection.
		auto view = REG().view<Core::Struct::Topology, Util::Math::AABB, Util::Math::Transform, System::Selection>();

		if ( view.size_hint() )
		{
			view.each(
				[ & ](
					const Entity &				   p_e,
					const Core::Struct::Topology & p_data,
					const Util::Math::AABB &	   p_aabb,
					const Util::Math::Transform &  p_transform,
					const System::Selection &	   p_selection
				)
				{
					if ( Helper::System::getSelectionState( { p_e, Core::Struct::E_SYSTEM_ITEM::SYSTEM } )
						 == System::E_SELECTION_STATE::FULL )
					{
						aabb.extend( p_aabb.transformed( p_transform ) );
					}
					// TODO: not recompute each time: cache values?
					else
					{
						Util::Math::AABB	   localAABB;
						std::span<const Vec3f> atomPositions = System::getCurrentAtomPositions( p_e );

						for ( auto atomIndex : p_selection.atoms )
						{
							localAABB.extend( atomPositions[ atomIndex ], Core::ChemDB::Atom::VDW_RADIUS_MIN );
						}

						if ( localAABB.isValid() )
						{
							aabb.extend( localAABB.transformed( p_transform ) );
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

	void SaveViewpoint::execute()
	{
		auto & reg = REG();

		const auto [ _, camera, transform ]
			= ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();

		Entity e = reg.create();
		reg.emplace<Util::Math::Transform>( e, transform );
		reg.emplace<Scene::ViewPoint>( e, camera.target );
		reg.emplace<App::Generic::Name>( e, DEFAULT_VIEWPOINT_NAME.data() );

		HUB().trigger<Events::ViewPointAdded>( e );
	}

	void SetViewPointPosition::execute( const Entity p_viewpoint, const Vec3f & p_position )
	{
		REG().patch<Util::Math::Transform>(
			p_viewpoint, [ &p_position ]( Util::Math::Transform & p_transform ) { p_transform.setPosition( p_position ); }
		);
	}

	void SetViewPointRotation::execute( const Entity p_viewpoint, const Quatf & p_rotation )
	{
		REG().patch<Util::Math::Transform>(
			p_viewpoint, [ &p_rotation ]( Util::Math::Transform & p_transform ) { p_transform.setRotation( p_rotation ); }
		);
	}

	void UpdateViewPointFromCamera::execute( const Entity p_viewpoint )
	{
		const auto [ _, camera, transform ]
			= ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();

		REG().patch<Util::Math::Transform>(
			p_viewpoint, [ &transform ]( Util::Math::Transform & p_transform ) { p_transform = transform; }
		);
		REG().patch<Scene::ViewPoint>(
			p_viewpoint, [ &camera ]( Scene::ViewPoint & p_viewpointData ) { p_viewpointData.target = camera.target; }
		);
	}

	void DeleteViewPoint::execute( const Entity p_viewpoint )
	{
		REG().destroy( p_viewpoint );
		HUB().trigger<Events::ViewPointDeleted>( p_viewpoint );
	}

	void GoToViewPoint::execute( const Entity p_viewpoint )
	{
		const auto & transform = REG().get<Util::Math::Transform>( p_viewpoint );
		const auto & viewpoint = REG().get<Scene::ViewPoint>( p_viewpoint );

		const auto cameraEntity = ECS::getFirstEntityOnlyWithComponents<Renderer::Camera>();
		REG().patch<Renderer::Camera>(
			cameraEntity, [ &viewpoint ]( Renderer::Camera & p_camera ) { p_camera.target = viewpoint.target; }
		);

		ACTION().execute<Animate<E_CAMERA_INTERPOLATOR::EASE_IN_OUT>>(
			transform.getPosition(), transform.getRotation()
		);
	}
} // namespace VTX::App::Action::Camera
