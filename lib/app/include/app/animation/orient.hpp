#ifndef __VTX_APP_ANIMATION_ORIENT__
#define __VTX_APP_ANIMATION_ORIENT__

#include "app/animation/concepts.hpp"
#include "app/animation/travel_manager.hpp"
#include "app/component/render/camera.hpp"
#include <util/hashing.hpp>
#include <util/math/aabb.hpp>
#include <util/types.hpp>

namespace VTX::App::Animation
{

	/**
	 * @brief Class responsible for managing the animation of focusing the camera on a target space
	 */
	class Orient
	{
	  public:
		inline static const float ORIENT_ZOOM_FACTOR = 0.666f;

		Orient( const App::Component::Render::Camera &, const Util::Math::AABB & );

		static Vec3f computeCameraOrientPosition(
			const Vec3f				 p_forward,
			const float				 p_fov,
			const Util::Math::AABB & p_target,
			const float				 p_orientZoomFactor = ORIENT_ZOOM_FACTOR
		);

		inline void	 update( const float p_delta, const float p_elasped ) { _animation.update( p_delta, p_elasped ); }
		inline void	 play() { _animation.play(); }
		inline void	 stop() { _animation.stop(); }
		inline float getRatio() const { return _animation.getRatio(); }
		inline bool	 isActive() const { return _animation.isActive(); }

		inline void subscribe( ProgressCallback p_ ) { _animation.onProgress += std::move( p_ ); }
		inline void subscribe( EndCallback p_ ) { _animation.onEnd += std::move( p_ ); }

	  private:
		TravelManager _animation;
	};
} // namespace VTX::App::Animation
#endif
