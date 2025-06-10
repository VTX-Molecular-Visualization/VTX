#ifndef __VTX_APP_ANIMATION_STRAIGHT_TRAVEL__
#define __VTX_APP_ANIMATION_STRAIGHT_TRAVEL__

#include "app/core/animation/concepts.hpp"
#include "app/core/animation/travel_manager.hpp"

namespace VTX::App::Animation
{
	/**
	 * @brief Class responsible for managing the animation of going to a certain point in a straight line.
	 */
	class StraightTravel
	{
	  public:
		enum class TravelRythm
		{
			linear,
			easeInOut
		};

		StraightTravel(
			const Vec3f &		p_finalPostion,
			const Quatf &		p_finalRotation,
			const float &		p_duration,
			const TravelRythm & p_rythm = TravelRythm::linear
		);

		inline void	 update( const float p_delta, const float p_elasped ) { _animation.update( p_delta, p_elasped ); }
		inline void	 play() { _animation.play(); }
		inline void	 stop() { _animation.stop(); }
		inline float getRatio() const { return _animation.getRatio(); }
		inline bool	 isActive() const { return _animation.isActive(); }

		inline void subscribe( Core::Animation::ProgressCallback p_ ) { _animation.onProgress += std::move( p_ ); }
		inline void subscribe( Core::Animation::EndCallback p_ ) { _animation.onEnd += std::move( p_ ); }

	  private:
		Core::Animation::TravelManager _animation;
	};
} // namespace VTX::App::Animation
#endif
