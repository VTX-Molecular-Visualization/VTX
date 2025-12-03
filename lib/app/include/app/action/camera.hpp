#ifndef __VTX_APP_ACTION_CAMERA__
#define __VTX_APP_ACTION_CAMERA__

#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/pass/controller/animation.hpp"
#include "app/services.hpp"
#include "app/settings/settings.hpp"
#include "app/settings/settings_manager.hpp"
#include <renderer/camera.hpp>
#include <util/math/aabb.hpp>
#include <util/math/transform.hpp>

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
	template<Renderer::PROJECTION P>
	struct SetProjectionMode
	{
		void execute()
		{
			auto & reg = REG();

			auto [ entity, camera ] = ECS::getFirstEntityWithComponents<Renderer::Camera>();

			reg.patch<Renderer::Camera>(
				entity,
				[]( Renderer::Camera & c )
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
		{
			execute( Pass::Controller::AnimationData { p_position, p_rotation }, p_duration );
		}

		void execute(
			const Pass::Controller::AnimationData & p_end,
			const float								p_duration = ANIMATION_DURATION_DEFAULT_MS
		)
		{
			using namespace Util;
			using namespace Pass::Controller;

			auto [ entCamera, camera, transform ]
				= ECS::getFirstEntityWithComponents<Renderer::Camera, Util::Math::Transform>();

			AnimationData	   start { transform.getPosition(), transform.getRotation() };
			InterpPositionFunc interpPositionFunc = nullptr;
			InterpRotationFunc interpRotationFunc = nullptr;

			// Check if animation is needed.
			if ( Math::distance( start.position, p_end.position ) < ANIMATION_TRANSLATION_THRESHOLD
				 && start.rotation == p_end.rotation )
			{
				return;
			}

			// Check existing animation pass.
			if ( PASS().hasPass<Pass::Controller::Animation>() )
			{
				PASS().removePass<Pass::Controller::Animation>();
			}

			// Select interpolation functions.
			if constexpr ( I == E_CAMERA_INTERPOLATOR::LINEAR )
			{
				interpPositionFunc = Math::lerp;
				interpRotationFunc = Math::lerp;
			}
			else if constexpr ( I == E_CAMERA_INTERPOLATOR::EASE_IN_OUT )
			{
				interpPositionFunc = Math::easeInOutInterpolation;
				interpRotationFunc = Math::easeInOutInterpolation;
			}
			else
			{
				static_assert( std::is_same_v<I, void>, "Unsupported camera interpolator." );
			}

			// Add pass.
			PASS().addPass<Animation>( entCamera, start, p_end, p_duration, interpPositionFunc, interpRotationFunc );
		}
	};

} // namespace VTX::App::Action::Camera

#endif
