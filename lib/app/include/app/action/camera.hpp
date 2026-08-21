#ifndef __VTX_APP_ACTION_CAMERA__
#define __VTX_APP_ACTION_CAMERA__

#include "app/controller/animation.hpp"
#include "app/controller/freefly.hpp"
#include "app/controller/trackball.hpp"
#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/scene/viewpoint.hpp"
#include "app/services.hpp"
#include <renderer/camera.hpp>
#include <util/math/aabb.hpp>
#include <util/math/transform.hpp>
#include <util/type_traits.hpp>

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
	 * @brief Set camera rotation.
	 */
	struct SetRotation
	{
		void execute( const Quatf & );
	};

	/**
	 * @brief Set camera field of view.
	 */
	struct SetFov
	{
		void execute( const float );
	};

	/**
	 * @brief Set camera near clipping plane.
	 */
	struct SetNearClip
	{
		void execute( const float );
	};

	/**
	 * @brief Set camera far clipping plane.
	 */
	struct SetFarClip
	{
		void execute( const float );
	};

	/**
	 * @brief Set camera projection mode.
	 */
	template<Renderer::PROJECTION P>
	struct SetProjectionMode
	{
		void execute()
		{
			auto &	   reg	  = REG();
			const auto entity = ECS::getFirstEntityOnlyWithComponents<Renderer::Camera>();

			reg.patch<Renderer::Camera>( entity, []( Renderer::Camera & p_cam ) { p_cam.projection = P; } );
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
	 * @brief Default duration in milliseconds.
	 */
	constexpr float ANIMATION_DURATION_DEFAULT_MS = 500.f;

	/**
	 * @brief Minimum translation distance to trigger animation.
	 */
	constexpr float ANIMATION_TRANSLATION_THRESHOLD = 0.1f;

	/**
	 * @brief Available camera interpolators.
	 */
	enum struct E_CAMERA_INTERPOLATOR
	{
		LINEAR,
		EASE_IN_OUT
	};

	/**
	 * @brief Animate camera to target position and rotation.
	 */
	template<E_CAMERA_INTERPOLATOR I = E_CAMERA_INTERPOLATOR::EASE_IN_OUT>
	struct Animate
	{
		void execute(
			const Vec3f & p_position,
			const Quatf & p_rotation,
			const float	  p_duration = ANIMATION_DURATION_DEFAULT_MS
		)
		{ execute( App::Controller::AnimationData { p_position, p_rotation }, p_duration ); }

		void execute(
			const App::Controller::AnimationData & p_end,
			const float							   p_duration = ANIMATION_DURATION_DEFAULT_MS
		)
		{
			using namespace Util;
			using namespace App::Controller;

			const auto [ entCamera, _, transform ]
				= ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();

			AnimationData	   start { transform.getPosition(), transform.getRotation() };
			InterpPositionFunc interpPositionFunc = nullptr;
			InterpRotationFunc interpRotationFunc = nullptr;

			// Check if animation is needed.
			if ( Math::distance( start.position, p_end.position ) < ANIMATION_TRANSLATION_THRESHOLD
				 && start.rotation == p_end.rotation )
			{
				// HUB().trigger<Events::CameraAnimationEnd>();
				return;
			}

			// Select interpolation functions.
			if constexpr ( I == E_CAMERA_INTERPOLATOR::LINEAR )
			{
				interpPositionFunc = Math::lerp;
				interpRotationFunc = Math::slerp;
			}
			else if constexpr ( I == E_CAMERA_INTERPOLATOR::EASE_IN_OUT )
			{
				interpPositionFunc = Math::easeInOutInterpolation;
				interpRotationFunc = Math::easeInOutInterpolation;
			}
			else
			{
				static_assert( always_false_v<I>, "Unsupported camera interpolator." );
			}

			// Run animation.
			HUB().trigger<Events::CameraAnimationStart>(
				start, p_end, p_duration, interpPositionFunc, interpRotationFunc
			);
		}
	};

	/**
	 * @brief Default viewpoint name.
	 */
	constexpr std::string_view DEFAULT_VIEWPOINT_NAME = "Viewpoint";

	/**
	 * @brief Save current camera position and orientation as a viewpoint.
	 */
	struct SaveViewpoint
	{
		void execute();
	};

	/**
	 * @brief Set viewpoint position.
	 */
	struct SetViewPointPosition
	{
		void execute( const Entity, const Vec3f & );
	};

	/**
	 * @brief Set viewpoint rotation.
	 */
	struct SetViewPointRotation
	{
		void execute( const Entity, const Quatf & );
	};

	/**
	 * @brief Update viewpoint from current camera.
	 */
	struct UpdateViewPointFromCamera
	{
		void execute( const Entity );
	};

	/**
	 * @brief Rename a viewpoint.
	 */
	struct RenameViewPoint
	{
		void execute( const Entity, const std::string_view );
	};

	/**
	 * @brief Delete a viewpoint.
	 */
	struct DeleteViewPoint
	{
		void execute( const Entity );
	};

	/**
	 * @brief Move camera to a viewpoint.
	 */
	struct GoToViewPoint
	{
		void execute( const Entity );
	};

} // namespace VTX::App::Action::Camera

#endif
