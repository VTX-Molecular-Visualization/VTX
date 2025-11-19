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

			auto [ entCamera, camera, transform ]
				= ECS::getFirstEntityWithComponents<App::Scene::Camera, Util::Math::Transform>();

			// Check existing animation pass.
			if ( PASS().hasPass<Pass::Controller::Animation>() )
			{
				PASS().removePass<Pass::Controller::Animation>();
			}

			Pass::Controller::InterpPositionFunc interpPositionFunc = nullptr;
			Pass::Controller::InterpRotationFunc interpRotationFunc = nullptr;

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
			PASS().addPass<Pass::Controller::Animation>(
				entCamera,
				Pass::Controller::AnimationData { transform.getPosition(), transform.getRotation() },
				p_end,
				p_duration,
				interpPositionFunc,
				interpRotationFunc
			);

			// Delete when done.
			auto c = HUB().connect<Events::CameraAnimationEnd>(
				[ entCamera ]()
				{
					// Remove pass.
					PASS().removePass<Pass::Controller::Animation>();
					// Disconnect.
					// HUB().disconnect( c );
				}
			);
		}
	};

} // namespace VTX::App::Action::Camera

#endif
