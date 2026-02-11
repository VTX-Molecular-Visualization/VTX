#ifndef __VTX_APP_CONTROLLER_ANIMATION__
#define __VTX_APP_CONTROLLER_ANIMATION__

#include "app/controller/concepts.hpp"
#include "app/ecs.hpp"

namespace VTX::App::Controller
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
	 * @brief Object that manages a camera animation to a target position and rotation.
	 */
	class Animation : public IController
	{
	  public:
		/**
		 * @brief Constructor.
		 */
		Animation(
			const AnimationData &,
			const AnimationData &,
			const float,
			const InterpPositionFunc &,
			const InterpRotationFunc &
		);

		/**
		 * @brief Called each frame.
		 */
		void update( const float, Util::Math::Transform & );

	  private:
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
} // namespace VTX::App::Controller

#endif
