#ifndef __VTX_APP_PASS_CONTROLLER_ANIMATION__
#define __VTX_APP_PASS_CONTROLLER_ANIMATION__

#include "app/ecs.hpp"
#include "app/pass/pass_manager.hpp"
#include "app/services.hpp"
#include "util/math/interpolators.hpp"
#include <util/math.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Pass::Controller
{
	/**
	 * @brief Final position and rotation.
	 */
	struct AnimationData
	{
		Vec3f position;
		Quatf rotation;
	};

	/**
	 * @brief Concept that defines a camera interpolator function (position + rotation).
	 */
	template<typename F>
	concept CameraInterpolator = requires(
		F			  p_func,
		const Vec3f & p_p0,
		const Vec3f & p_p1,
		const Quatf & p_q0,
		const Quatf & p_q1,
		float		  p_t
	) {
		{ p_func( p_p0, p_p1, p_t ) } -> std::same_as<Vec3f>;
		{ p_func( p_q0, p_q1, p_t ) } -> std::same_as<Quatf>;
	};

	/**
	 * @brief Default duration in milliseconds.
	 */
	constexpr float ANIMATION_TRANSLATION_THRESHOLD = 0.1f;

	/**
	 * @brief System that manages a camera animation to a target position and rotation.
	 */
	template<CameraInterpolator Interp>
	class Animation : public IPass
	{
	  public:
		/**
		 * @brief Constructor.
		 */
		Animation(
			const ECS::Entity &	  p_ent,
			const AnimationData & p_dataStart,
			const AnimationData & p_dataEnd,
			const float			  p_duration
		) :
			_cameraEntity( p_ent ), _animationDataStart( p_dataStart ), _animationDataEnd( p_dataEnd ),
			_duration( p_duration )
		{
			using namespace Util;

			const float translationDistance
				= Math::distance( _animationDataStart.position, _animationDataEnd.position );
			const bool skipAnimation = translationDistance < ANIMATION_TRANSLATION_THRESHOLD
									   && _animationDataStart.rotation == _animationDataEnd.rotation;

			// Skip at first update.
			if ( skipAnimation )
			{
				_finished = true;
				return;
			}

			_time = 0.f;

			// Set initial position and rotation.
			REG().patch<Math::Transform>(
				_cameraEntity,
				[ & ]( Math::Transform & p_transform )
				{
					p_transform.setPosition( _animationDataStart.position );
					p_transform.setRotation( _animationDataStart.rotation );
				}
			);
		}

		/**
		 * @brief Called each frame.
		 */
		void update( const float p_delta, const float p_elapsed )
		{
			using namespace Util;

			if ( _finished )
			{
				HUB().enqueue<Events::CameraAnimationEnd>();
				return;
			}

			REG().patch<Math::Transform>(
				_cameraEntity,
				[ this, p_delta ]( Math::Transform & p_transform )
				{
					_time += p_delta;

					// Lerp.
					const float t = Math::clamp( _time / _duration, 0.f, 1.f );
					p_transform.setPosition( _interp( _animationDataStart.position, _animationDataEnd.position, t ) );
					p_transform.setRotation( _interp( _animationDataStart.rotation, _animationDataEnd.rotation, t ) );

					// Auto remove.
					if ( t >= 1.f )
					{
						_finished = true;
					}
				}
			);
		}

	  private:
		/**
		 * @brief Controlled camera entity.
		 */
		const ECS::Entity _cameraEntity;

		/**
		 * @brief Start and end animation data.
		 */
		const AnimationData _animationDataStart;
		const AnimationData _animationDataEnd;

		/**
		 * @brief Animation duration.
		 */
		const float _duration;

		/**
		 * @brief Elapsed time since the beginning of the animation.
		 */
		float _time = 0.f;

		/**
		 * @brief Animation finished, trigger callback.
		 */
		bool _finished = false;

		/**
		 * @brief Interpolation function.
		 */
		Interp _interp;
	};
} // namespace VTX::App::Pass::Controller

#endif
