#include "app/action/camera.hpp"
#include "app/action/action_manager.hpp"
#include "app/helper/system.hpp"
#include "app/scene/tag_root.hpp"
#include "app/system/selection.hpp"
#include "app/system/trajectory.hpp"
#include <core/struct/system.hpp>
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
		const auto & [ ent, _, __ ] = ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();
		REG().patch<Util::Math::Transform>(
			ent, [ p_position ]( Util::Math::Transform & p_transform ) { p_transform.setPosition( p_position ); }
		);
	}

	void SetRotation::execute( const Vec3f & p_eulerAngles )
	{
		const auto & [ ent, _, __ ] = ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();
		REG().patch<Util::Math::Transform>(
			ent, [ p_eulerAngles ]( Util::Math::Transform & p_transform ) { p_transform.setRotation( p_eulerAngles ); }
		);
	}

	void SetScale::execute( const float p_scale )
	{
		const auto & [ ent, _, __ ] = ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();
		REG().patch<Util::Math::Transform>(
			ent, [ p_scale ]( Util::Math::Transform & p_transform ) { p_transform.setScale( p_scale ); }
		);
	}

	void Reset::execute()
	{
		const auto	 entScene = ECS::getFirstEntityOnlyWithComponents<App::Scene::TagRoot, Util::Math::AABB>();
		const auto & aabb	  = REG().get<Util::Math::AABB>( entScene );
		const auto & [ entCamera, camera, _ ]
			= ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();
		REG().patch<Util::Math::Transform>(
			entCamera,
			[ & ]( Util::Math::Transform & p_transform )
			{
				Vec3f position = _computeCameraOrientPosition( FRONT_AXIS, camera.fov, aabb );

				p_transform.setPosition( position );
				p_transform.setRotation( QUATF_ID );
				p_transform.lookAt( aabb.centroid() );
			}
		);

		// Change controller target.
		if ( PASS().hasPass<Pass::Controller::Trackball>() )
		{
			PASS().getPass<Pass::Controller::Trackball>()->setTarget( aabb.centroid() );
		}
	}

	void Orient::execute()
	{
		const auto & [ entCamera, _, __ ]
			= ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();

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

		const auto & [ _, camera, transform ]
			= ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();

		// Change controller target.
		if ( PASS().hasPass<Pass::Controller::Trackball>() )
		{
			PASS().getPass<Pass::Controller::Trackball>()->setTarget( p_target.centroid() );
		}

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
