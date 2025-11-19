#ifndef __VTX_APP_ACTION_CAMERA__
#define __VTX_APP_ACTION_CAMERA__

#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/pass/controller/animation.hpp"
#include "app/scene/camera.hpp"
#include "app/services.hpp"
#include "app/settings/settings.hpp"
#include "app/settings/settings_manager.hpp"
#include <util/math/aabb.hpp>

namespace VTX::App::Action::Camera
{
	/**
	 * @brief Set camera position.
	 */
	struct SetPosition
	{
		void execute( const Vec3f & );
	};

	/**
	 * @brief Set camera rotation (euler angles).
	 */
	struct SetRotation
	{
		void execute( const Vec3f & );
	};

	/**
	 * @brief Set camera scale.
	 */
	struct SetScale
	{
		void execute( const float );
	};

	/**
	 * @brief Set camera projection mode.
	 */
	template<App::Scene::Camera::PROJECTION P>
	struct SetProjectionMode
	{
		void execute()
		{
			auto & reg = REG();

			auto [ entity, camera ] = ECS::getFirstEntityWithComponents<App::Scene::Camera>();

			reg.patch<App::Scene::Camera>(
				entity,
				[]( App::Scene::Camera & c )
				{
					auto & settings = SETTINGS();
					settings.setValue<int>( Settings::Camera::PROJECTION_KEY, int( P ) );
				}
			);

			static constexpr int PROJ_INDEX = static_cast<int>( P );
			HUB().trigger<App::Events::CameraProjectionChange<PROJ_INDEX>>();
		}
	};

	/**
	 * @brief Reset camera instantanly to fit the scene.
	 */
	struct Reset
	{
		void execute();
	};

	/**
	 * @brief Launch animation to orient the camera to fit the target.
	 */
	struct Orient
	{
		/**
		 * @brief Orient on current selection, or scene AABB if no selection.
		 */
		void execute();
		/**
		 * @brief Orient on given AABB.
		 */
		void execute( const Util::Math::AABB & p_target );
	};

	/**
	 * @brief Launch animation to travel in a straight line to the target position and rotation.
	 */
	struct StraightTravel
	{
		void execute( const Vec3f & p_position, const Quatf & p_rotation, const float p_duration );
	};

	/**
	 * @brief Launch animation to travel to destination with given interpolator.
	 */
	constexpr float ANIMATION_DURATION_DEFAULT_MS = 500.f;
	template<Pass::Controller::CameraInterpolator I = Util::Math::Interpolators::EaseInOut>
	struct Animate
	{
		void execute(
			const Vec3f & p_position,
			const Quatf & p_rotation,
			const float	  p_duration = ANIMATION_DURATION_DEFAULT_MS
		)
		{
			execute( Pass::Controller::AnimationData { p_position, p_rotation }, p_duration );
		}

		void execute(
			const Pass::Controller::AnimationData & p_end,
			const float								p_duration = ANIMATION_DURATION_DEFAULT_MS
		)
		{
			using namespace Util;

			auto [ entCamera, camera, transform ]
				= ECS::getFirstEntityWithComponents<App::Scene::Camera, Util::Math::Transform>();

			// Check if this animation is running.
			if ( REG().all_of<AnimationRunningTag>( entCamera ) )
			{
				return;
			}
			REG().emplace<AnimationRunningTag>( entCamera );

			// Add pass.
			PASS().addPass<Pass::Controller::Animation<I>>(
				entCamera,
				Pass::Controller::AnimationData { transform.getPosition(), transform.getRotation() },
				p_end,
				p_duration
			);

			// Delete when done.
			auto c = HUB().connect<Events::CameraAnimationEnd>(
				[ entCamera ]()
				{
					// Remove pass.
					PASS().removePass<Pass::Controller::Animation<I>>();
					// Remove tag.
					REG().remove<AnimationRunningTag>( entCamera );
					// Disconnect.
					// HUB().disconnect( c );
				}
			);
		}

		/**
		 * @brief Tag to know if an animation is running.
		 */
		struct AnimationRunningTag
		{
		};
	};

} // namespace VTX::App::Action::Camera

#endif
