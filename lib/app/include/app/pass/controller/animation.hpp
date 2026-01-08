#ifndef __VTX_APP_PASS_CONTROLLER_ANIMATION__
#define __VTX_APP_PASS_CONTROLLER_ANIMATION__

#include "app/ecs.hpp"
#include "app/pass/pass_manager.hpp"

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
	 * @brief Interpolation functors.
	 */
	using InterpPositionFunc = Vec3f ( * )( const Vec3f &, const Vec3f &, float );
	using InterpRotationFunc = Quatf ( * )( const Quatf &, const Quatf &, float );

	/**
	 * @brief System that manages a camera animation to a target position and rotation.
	 */
	class Animation : public IPass
	{
	  public:
		/**
		 * @brief Constructor.
		 */
		Animation(
			const ECS::Entity &,
			const AnimationData &,
			const AnimationData &,
			const float,
			const InterpPositionFunc &,
			const InterpRotationFunc &
		);

		/**
		 * @brief Called each frame.
		 */
		void update( const float, const float );

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
		 * @brief Interpolation functions.
		 */
		const InterpPositionFunc _interpPosition;
		const InterpRotationFunc _interpRotation;
	};
} // namespace VTX::App::Pass::Controller

#endif
